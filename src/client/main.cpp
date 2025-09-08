#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
/* You will to add includes here */

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG 1


// Included to get the support library
#include <calcLib.h>

int main(int argc, char *argv[]){
  for (int i = 0; i < argc; i++)
  {
    printf("%s", argv[i]);
  }
  printf("\n");

  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<DNS|IPv4|IPv6>:<PORT>], got %d arguments\n", argc);
    fflush(stdout);
    exit(-1);
  }

  char* destination;
  char* destinationPort; 
  int returnValue = split_ip_port_from_string(argv[1], &destination, &destinationPort);
  if (returnValue < 0)
  {
    printf("Did not find viable IP and PORT, exiting...\n");
    exit(-1);
  }

  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* results;

  printf("Host %s, and port %s.\n",destination, destinationPort);
  returnValue = getaddrinfo(
    destination,
    destinationPort,
    &hints,
    &results
  );
  if (returnValue < 0)
  {
    return returnValue;
  }
  freeaddrinfo(results);
  free(destination);
  free(destinationPort);
  return 0;
}
