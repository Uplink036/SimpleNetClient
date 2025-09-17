#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "netparser.h"


void validate_input_args(int argc, char *argv[])
{
  DEBUG_FUNCTION("client::main::validate_input_args(&d, ...)\n", argc);
  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<DNS|IPv4|IPv6>:<PORT>], got %d arguments\n", argc);
    exit(-1);
  }
  if (strstr(argv[1], "///") != NULL ){
    printf("Invalid format: %s.\n", argv[1]);
    exit(-1); 
  }
  if (strstr(argv[1], "://") != NULL) {
    printf("Invalid format: missing '://'\n");
    exit(-1);
  }
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);
  
  char protocolstring[6], pathstring[7];
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  char* destination;
  char* destinationPort; 
  getIPnPORT(argv[1], &destination, &destinationPort);

  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* results;

  printf("Host %s, and port %s.\n",destination, destinationPort);
  int returnValue = getaddrinfo(
    destination,
    destinationPort,
    &hints,
    &results
  );
  if (returnValue < 0)
  {
    return returnValue;
  }

  struct addrinfo *rp;
  int socketfd;
  for (rp = results; rp != NULL; rp = rp->ai_next) {
    socketfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
    if (socketfd == -1)
        continue;

    if (connect(socketfd, rp->ai_addr, rp->ai_addrlen) == 0)
        break;
  }
  freeaddrinfo(results);
  close(socketfd);
  free(destination);
  free(destinationPort);
  return 0;
}
