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
  char delim = ':';
  int i;
  int inputLen = strlen(argv[1]);
  for (i = inputLen; i > 0; i--)
  {
    if (argv[1][i] == delim)
    {
      int destinationLength = i;
      int portLength = inputLen - (i);
      destination = (char*)malloc(sizeof(destinationLength)*sizeof(*destination));
      strncpy(destination, argv[1], destinationLength);
      destinationPort = (char*)malloc(sizeof(portLength)*sizeof(*destinationPort));
      strncpy(destinationPort, argv[1]+i+1, portLength);
      break;
    }
  }
  if (i == 0)
  {
    printf("Did not find delim, exiting...");
    exit(-1);
  }

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
  freeaddrinfo(results);
  free(destination);
  free(destinationPort);
  return 0;
}
