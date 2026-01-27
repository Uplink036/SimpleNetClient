#include "src/client/argParse.h"

void parseInputArgs(
  char *argv[],
  char protocolstring[6],
  char pathstring[7],
  char **destination,
  char **destinationPort
) {
  DEBUG_FUNCTION("client::argParse::parseInputArgs(%s)\n", argv[1]);
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  getIPnPORT(argv[1], destination, destinationPort);
}

void validateInputArgs(int argc, char *argv[]) {
  DEBUG_FUNCTION("client::argParse::validateInputArgs(%d, ...)\n", argc);
  if (argc != 2) {
    printf("Unexpected amount of inputs, got %d arguments\n", argc);
    printf("Expected format: [<PROGRAM>] [<DNS|IPv4|IPv6>:<PORT>]\n");
    exit(-1);
  }
  IF_NOT_NULL(strstr(argv[1], "///")) {
    printf("Invalid format: %s.\n", argv[1]);
    exit(-1);
  }
  char* protoEnd = strstr(argv[1], "://");
  IF_NULL(protoEnd) {
    printf("Invalid format: missing '://'\n");
    exit(-1);
  }
  char* portStart = strstr(protoEnd+3, ":");
  IF_NULL(portStart) {
    printf("Invalid format: missing ':'\n");
    exit(-1);
  }
  char* apiStart = strstr(protoEnd+3, "/");
  IF_NULL(apiStart) {
    printf("Invalid format: missing '/'\n");
    exit(-1);
  }
}
