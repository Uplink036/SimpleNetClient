#include "src/server/protocol.h"

const char* SERVER_PROTOCOLS[] = {
  "TEXT TCP 1.1\n",
  "\n",
};

bool sendServerProtocol(int client_fd) {
  DEBUG_FUNCTION("server::protocol::sendServerProtocol(%d)\n", client_fd);
  int protocol_index = 0;
  do {
    const char* protocl = SERVER_PROTOCOLS[protocol_index];
    IF_NEGATIVE(send(client_fd, protocl, strlen(protocl), 0)) {
      perror("send");
      close(client_fd);
      return false;
    }
  }
  while (strcmp(SERVER_PROTOCOLS[protocol_index++], "\n") != 0);
  return true;
}

bool getClientProtocol(int client_fd, ApplicationProtocl* clientProtocol) {
  DEBUG_FUNCTION("server::protocol::getClientProtocol(%d, %p)\n", client_fd,
                 clientProtocol);
  char msg[MAX_MESSGE_SIZE];
  memset(msg, 0, MAX_MESSGE_SIZE);
  static const int max_buffer_size = sizeof(msg)-1;
  int readSize = recv(client_fd, &msg, max_buffer_size, 0);
  IF_NEGATIVE(readSize) {
      perror("recive");
      close(client_fd);
      return false;
  }

  IF_ZERO(strcmp(msg, "\n")) {
      perror("wrong protocol type");
      close(client_fd);
      return false;
  }

  int protocol_index = 0;
  while (!(strcmp(SERVER_PROTOCOLS[protocol_index], "\n") == 0)) {
    const char* protocl = SERVER_PROTOCOLS[protocol_index++];
    const int protocol_length = strlen(protocl)-1;
    IF_ZERO(strncmp(protocl, msg, protocol_length)) {
      IF_ZERO(strcmp(msg+protocol_length, " OK\n")) {
        clientProtocol->com = TEXT;
        return true;
      }
    }
  }
  DEBUG_FUNCTION("server::main::getClientProtocol - Found no matching "
                 "protocols for %s\n", msg);
  return false;
}
