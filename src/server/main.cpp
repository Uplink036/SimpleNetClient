#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "macros.h"

#define BACKLOG 2	 // how many pending connections queue will hold

void validate_input_args(int argc, char *argv[])
{
  DEBUG_FUNCTION("client::main::validate_input_args(&d, ...)\n", argc)
  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<PORT>], got %d arguments\n", argc);
    fflush(stdout);
    exit(-1);
  }
  if (!validPORT(argv[1]))
  {
    printf("Not a valid port, expected a value between 0-65535, got %s\n", argv[1]);
    fflush(stdout);
    exit(-1);
  }
}

void populateTCPHint(addrinfo* hints)
{
  memset(hints, 0, sizeof(addrinfo));
  hints->ai_family = AF_UNSPEC;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_socktype = SOCK_STREAM;
}

int bindPort(char* desiredPort, addrinfo** selectedIP)
{
  addrinfo hints;
  addrinfo* avaliableIPs;
  int returnValue;
  
  populateTCPHint(&hints);
  returnValue = getaddrinfo(NULL, desiredPort, &hints, &avaliableIPs);
	IF_NOT_ZERO(returnValue)
  {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(returnValue));
		return -1;
	}

  int socket_fd;
  addrinfo* p;
	for(p = avaliableIPs; p != NULL; p = p->ai_next)
  {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    IF_NEGATIVE(socket_fd)
      {
        perror("server: socket\n");
        continue;
      }
      
    const int yes=1;
		IF_NOT_ZERO(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
    {
      perror("setsockopt");
			return -1;
		}

		IF_NEGATIVE(bind(socket_fd, p->ai_addr, p->ai_addrlen)) 
    {
			close(socket_fd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(avaliableIPs);
  *selectedIP = p;
	IF_NULL(p)  {
		fprintf(stderr, "server: failed to bind\n");
		return -1;
	}

	IF_NEGATIVE(listen(socket_fd, BACKLOG)) {
		perror("listen");
		return -1;
	}  
  return socket_fd;
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);

  int socket_fd;
  addrinfo* serverIP;
  socket_fd = bindPort(argv[1], &serverIP);
  IF_NEGATIVE(socket_fd)
  {
    fprintf(stderr, "Could not bind to specified port\n");
    return socket_fd;
  }

  printf("server: waiting for connections...\n");
	char msg[1500];
	int MAXSZ=sizeof(msg)-1;
	int childCnt=0;
	int readSize;
	
  socklen_t socket_in_size;
  struct sockaddr_storage their_addr;
  char client[INET6_ADDRSTRLEN];
  int new_fd;
	while(true) {
    socket_in_size = sizeof(their_addr);
    new_fd = accept(socket_fd, (struct sockaddr *)&their_addr, &socket_in_size);
    IF_NEGATIVE(new_fd) 
    {
      perror("accept");
      continue;
    }

    getnameinfo(serverIP->ai_addr, serverIP->ai_addrlen, client, sizeof(client),
                              nullptr, 0, 0);
    printf("server: Connection %d from %s\n",childCnt, client);
    printf("server: Sending welcome \n");
    struct sockaddr_in *local_sin=(struct sockaddr_in*)&their_addr;
    IF_NEGATIVE(send(new_fd, "Hello, world!", 13, 0)){
      perror("send");
      close(new_fd);
      continue;
    }

    while(true){
      readSize=recv(new_fd,&msg,MAXSZ,0);
      printf("Child[%d] (%s:%d): recv(%d) .\n", childCnt,client,ntohs(local_sin->sin_port),readSize);
      IF_ZERO(readSize)
      {
        printf("Child [%d] died.\n",childCnt);
        close(new_fd);
        break;
      }
      msg[readSize]=0;
      send(new_fd,&msg,readSize,0);	 
    }
    printf("Socket closed()\n");
	}
	return 0;
}