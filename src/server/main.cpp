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


const char* SERVER_PROTOCOLS[] = {
  "TEXT TCP 1.1\n",
  "\n",

};

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

bool sendServerInformation(int client_fd)
{
  int protocol_index = 0;
  do 
  {
    const char* protocl = SERVER_PROTOCOLS[protocol_index];
    IF_NEGATIVE(send(client_fd, protocl,strlen(protocl), 0))
    {
      perror("send");
      close(client_fd);
      return false;
    }
  }
  while (strcmp(SERVER_PROTOCOLS[protocol_index++], "\n") == 0);
  return true;
}

#define MAX_MESSGE_SIZE 1500



enum CommunicationProtocl
{
  TEXT,
  BINARY
};

enum InternetProtocl
{
  TCP,
  UDP
};

struct ApplicationProtocl
{
  CommunicationProtocl com;
  InternetProtocl proc;
};

bool getClientProtocol(int client_fd, ApplicationProtocl* clientProtocol)
{
  char msg[MAX_MESSGE_SIZE];
  static const int max_buffer_size = sizeof(msg)-1;
  int readSize=recv(client_fd,&msg, max_buffer_size, 0);
  IF_NEGATIVE(readSize)
  {
      perror("recive");
      close(client_fd);
      return false;
  }

  IF_NOT_ZERO(strcmp(msg, "\n"))
  {
      perror("protocol type");
      close(client_fd);
      return false;
  }

  int protocol_index = 0;
  while (strcmp(SERVER_PROTOCOLS[protocol_index], "\n") == 0)
  {
    const char* protocl = SERVER_PROTOCOLS[protocol_index];
    const int protocol_length = strlen(protocl)-1;
    IF_ZERO(strncmp(protocl, msg, protocol_length))
    {
      IF_ZERO(strcmp(msg+protocol_length, "OK\n"))
      {
        clientProtocol->com = TEXT;
        return true;
      }
    }
  }
  return false;
}


bool sendClientTask(int client_fd)
{
  const char task[] = "add 1 1";
  IF_NEGATIVE(send(client_fd, &task, strlen(task), 0))
    return false;
  return true;
}

bool recvClientResponse(int client_fd)
{
  char msg[1500];
	static const int max_buffer_size= sizeof(msg)-1;
  int readSize=recv(client_fd,&msg,max_buffer_size,0);
  IF_NEGATIVE(readSize)
    return false;
  IF_NOT_ZERO(strncmp(msg, "2\n", readSize))
    return false;
  return true;
}

bool clientTask(int client_fd)
{
  if(sendClientTask(client_fd) EQUALS false)
  {
    close(client_fd);
    return false;
  }
  if(recvClientResponse(client_fd) EQUALS false)
  {
    close(client_fd);
    return false;
  }
  return true;
}

void listenOnPort(int socket_fd, addrinfo* serverIP)
{
  int childCnt=0;
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
    
    if (sendServerInformation(new_fd) EQUALS false)
      continue;
    ApplicationProtocl clientProtocol;
    if (getClientProtocol(new_fd, (&clientProtocol)) EQUALS false)
      continue;
    
    if (clientTask(new_fd) EQUALS false)
      continue;
    close(new_fd);
    printf("Socket closed()\n");
	}
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
		fprintf(stderr, "server: Failed to bind\n");
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

  printf("server: Waiting for connections...\n");
	listenOnPort(socket_fd, serverIP);
  close(socket_fd);
  freeaddrinfo(serverIP);
  return 0;
}