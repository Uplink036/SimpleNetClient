#include "argParse.h"

void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort)
{
  DEBUG_FUNCTION("client::main::parseInputArgs(%s)\n", argv[1]);
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  getIPnPORT(argv[1], &destination, &destinationPort);
}

void validateInputArgs(int argc, char *argv[])
{
  DEBUG_FUNCTION("client::main::validateInputArgs(%d, ...)\n", argc);
  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<DNS|IPv4|IPv6>:<PORT>], got %d arguments\n", argc);
    exit(-1);
  }
  if (strstr(argv[1], "///") != NULL ){
    printf("Invalid format: %s.\n", argv[1]);
    exit(-1); 
  }
  if (strstr(argv[1], "://") == NULL) {
    printf("Invalid format: missing '://'\n");
    exit(-1);
  }
}
