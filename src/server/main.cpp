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

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);
  
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* serverResults;

  int rv;
	if ((rv = getaddrinfo(NULL, argv[1], &hints, &serverResults)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
  int sockfd, new_fd;
  addrinfo *p;
	for(p = serverResults; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
      {
        perror("server: socket");
        continue;
      }
      
    const int yes=1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
    {
      perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
    {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
	freeaddrinfo(serverResults); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}  
  printf("server: waiting for connections...\n");
	char msg[1500];
	int MAXSZ=sizeof(msg)-1;
	int childCnt=0;
	int readSize;
	char command[10];
	char optionstring[128];
	int optionint1;
	int optionint2;
	
  socklen_t socket_in_size;
  struct sockaddr_storage their_addr;
  char client[INET6_ADDRSTRLEN];
	while(true) {
    socket_in_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &socket_in_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }
    getnameinfo(p->ai_addr, p->ai_addrlen, client, sizeof(client),
                              nullptr, 0, 0);
    printf("server: Connection %d from %s\n",childCnt, client);
    
    printf("server: Sending welcome \n");
    struct sockaddr_in *local_sin=(struct sockaddr_in*)&their_addr;
    if (send(new_fd, "Hello, world!", 13, 0) == -1){
      perror("send");
      close(new_fd);
      continue;
    }
    while(true){
      readSize=recv(new_fd,&msg,MAXSZ,0);
      printf("Child[%d] (%s:%d): recv(%d) .\n", childCnt,client,ntohs(local_sin->sin_port),readSize);
      if(readSize==0){
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