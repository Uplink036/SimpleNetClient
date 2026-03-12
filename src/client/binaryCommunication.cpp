#include "src/client/binaryCommunication.h"

bool getBinaryServerProtocols(int socketfd, char* expected_protocol,
                            fd_set* fdset, timeval* tv) {
  DEBUG_FUNCTION("client::binaryCommunication::getBinaryServerProtocols(%d, ...)\n", socketfd);
  calcMessage* firstMessage = (calcMessage*)calloc(1, sizeof(*firstMessage));
  firstMessage->message = 0;

  if (strcmp(expected_protocol, "UDP"))
    firstMessage->protocol = 17;

  else
   firstMessage->protocol = 6;
  firstMessage->major_version = 1;
  firstMessage->minor_version = 0;
}