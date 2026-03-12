#include "src/client/main.h"

int main(int argc, char *argv[]) {
  validateInputArgs(argc, argv);
  int exitStatus = 0;
  char protocolstring[6], pathstring[7];
  char* destination;
  char* destinationPort;
  parseInputArgs(argv, protocolstring, pathstring, &destination,
                 &destinationPort);
  if (strcmp(protocolstring, "TCP") == 0) {
    exitStatus = connectTCP(destination, destinationPort, pathstring,
                            protocolstring);
  } else if (strcmp(protocolstring, "UDP") == 0) {
    exitStatus = connectUDP(destination, destinationPort, pathstring,
                            protocolstring);
  } else {
    printf("ERROR: UNSUPPORTED TRANSPORT PROTOCOL\n");
    exitStatus = 1;
  }
  free(destination);
  free(destinationPort);
  return exitStatus;
}
