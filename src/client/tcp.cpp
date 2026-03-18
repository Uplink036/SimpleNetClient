#include "src/client/tcp.h"

static int getServerTask(int socketfd, char* msg) {
  DEBUG_FUNCTION("client::tcp::getServerTask(%d %s)\n", socketfd, msg);
  static const int max_buffer_size = 1499;
  memset(msg, 0, 1500);
  int readSize = recv(socketfd, msg, max_buffer_size, 0);
  DEBUG_FUNCTION("client::tcp::getServerTask - Got %d bytes = %s",
                 readSize, msg);
  return readSize;
}

static bool getServerProtocols(int socketfd, char* expected_protocol,
                               fd_set* fdset, timeval* tv) {
  DEBUG_FUNCTION("client::tcp::getServerProtocols(%d, ...)\n", socketfd);
  bool foundProtocl = false;
  char msg[1500];
  static const int max_buffer_size = sizeof(msg)-1;
  int loop = 0;
  do {
    memset(msg, 0, 1500);
    DEBUG_FUNCTION("client::tcp::fromServer - Waiting %d\n", loop++);
    int readSize = recv(socketfd, &msg, max_buffer_size, 0);
    if (select(socketfd + 1, fdset, NULL, NULL, tv) == 1) {
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
          int readSize = recv(socketfd, &msg, max_buffer_size, 0);
          DEBUG_FUNCTION("client::tcp::fromServer - Received %d bytes = %s",
                         readSize, msg);
          IF_NEGATIVE(readSize)
            return false;
          DEBUG_FUNCTION("client::tcp::fromServer - Looking for - %s",
                         expected_protocol);
          if (strstr(msg, expected_protocol) NOTEQUALS NULL) {
            foundProtocl = true;
            break;
          }
        }
    } else {
      break;
    }
  } while (msg[0] != '\n' AND loop < 2000);
  DEBUG_FUNCTION("Got task %s ", msg);
  fflush(stdout);
  return foundProtocl;
}

static int sendClientProtocol(bool foundProtocl, int socketfd,
                              char pathstring[7], char protocolstring[6]) {
  DEBUG_FUNCTION("client::tcp::sendClientProtocol(%d, %d, %s, %s)\n",
                  foundProtocl, socketfd, pathstring, protocolstring);
  if (NOT foundProtocl) {
    char errorMessage[] = "ERROR\n";
    return send(socketfd, errorMessage, strlen(errorMessage), 0);
  } else {
    char successMessage[100];
    snprintf(successMessage, sizeof(successMessage), "%s %s 1.1 OK\n",
             pathstring, protocolstring);
    return send(socketfd, successMessage, strlen(successMessage), 0);
  }
}

static int sendResultToServer(int result, int socketfd) {
  DEBUG_FUNCTION("client::tcp::sendResultToServer(%d, %d)\n",
                 result, socketfd);
  char resultMessage[100];
  memset(resultMessage, 0, sizeof(resultMessage));
  formatTextResult(result, resultMessage, sizeof(resultMessage));
  return send(socketfd, resultMessage, strlen(resultMessage), 0);
}

static void getResultResponseBack(int socketfd, int result) {
  DEBUG_FUNCTION("client::tcp::getResultResponseBack(%d)\n", socketfd);
  char msg[1500];
  memset(msg, 0, 1500);
  static const int max_buffer_size = sizeof(msg)-1;
  recv(socketfd, &msg, max_buffer_size, 0);
  if (strcmp(msg, "OK\n") == 0)
    printf("OK (myresult=%d)\n", result);
  else
    printf("Fail myresult=%d, server response %s", result, msg);
}

int connectTCP(char* destination, char* destinationPort,
               char pathstring[7], char protocolstring[6]) {
  DEBUG_FUNCTION("client::tcp::connectTCP(%s, %s, %s, %s)\n",
    destination, destinationPort, pathstring, protocolstring);
  int exitStatus = 0;
  fd_set fdset;
  struct timeval tv;
  FD_ZERO(&fdset);
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  bool foundServer = false;
  bool timedOut = true;

  addrinfo hints;
  populateTCPHint(&hints);

  addrinfo* results;
  printf("Host %s, and port %s.\n", destination, destinationPort);
  int returnValue = getaddrinfo(destination, destinationPort, &hints,
                                &results);
  if (results == NULL OR returnValue < 0) {
    printf("ERROR:");
    exitStatus = 1;
    goto freeTCP;
  }

  struct addrinfo *rp;
  int socketfd;
  for (rp = results; rp != NULL; rp = rp->ai_next) {
    DEBUG_FUNCTION("Testing socket %p\n", rp);
    socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (socketfd == -1) {
      perror("Problem with the socket: ");
      continue;
    }
    int flags = fcntl(socketfd, F_GETFL);
    fcntl(socketfd, F_SETFL, O_NONBLOCK);
    connect(socketfd, rp->ai_addr, rp->ai_addrlen);
    FD_SET(socketfd, &fdset);
    if (select(socketfd + 1, NULL, &fdset, NULL, &tv) == 1) {
      int so_error;
      socklen_t len = sizeof so_error;
      getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
      if (so_error == 0) {
        timedOut = false;
        DEBUG_FUNCTION("Testing connection %c\n", rp);
        fflush(stdout);
        foundServer = true;
        char expected_protocol[100];
        snprintf(expected_protocol, sizeof(expected_protocol),
                 "%s %s 1.1\n", pathstring, protocolstring);
        bool foundProtocol = getServerProtocols(socketfd,
                                                expected_protocol,
                                                &fdset, &tv);
        if (NOT foundProtocol) {
          printf("ERROR: NO PROTOCOL FOUND (TIMEOUT)\n");
          DEBUG_FUNCTION("Failed to get a protocol from server after "
                         "%d checks\n", 2000);
          exitStatus = 1;
          goto freeTCP;
        }
        fcntl(socketfd, F_SETFL, flags & ~O_NONBLOCK);
        IF_NEGATIVE(sendClientProtocol(foundProtocol, socketfd,
                                       pathstring, protocolstring)) {
          printf("ERROR: COULD NOT SEND PROTOCOL OK\n");
          DEBUG_FUNCTION("Could not send positive protocol to server "
                         "%d\n", 0);
          exitStatus = 1;
          goto freeTCP;
        }
        char msg[1500];
        IF_NEGATIVE(getServerTask(socketfd, msg)) {
          printf("ERROR: COULD NOT SEND TASK TO SERVER (TIMEOUT)\n");
          DEBUG_FUNCTION("Could not get task from server %d\n", 0);
          exitStatus = 1;
          goto freeTCP;
        }
        int result = calculateTextTask(msg);
        IF_NEGATIVE(sendResultToServer(result, socketfd)) {
          printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
          DEBUG_FUNCTION("Could not send result back to server %d\n", 0);
          exitStatus = 1;
          goto freeTCP;
        }
        getResultResponseBack(socketfd, result);
        break;
      }
    } else {
      break;
    }
  }

  if (foundServer EQUALS false) {
    if (timedOut EQUALS true) {
      printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
      DEBUG_FUNCTION("Found no server to connect to on ip %s.\n",
                     destination);
      exitStatus = 1;
    } else {
      printf("ERROR: COULD NOT FIND A SERVER (TIMEOUT)\n");
      DEBUG_FUNCTION("Found no server to connect to on ip %s.\n",
                     destination);
      exitStatus = 1;
    }
  }

  freeTCP:
  fflush(stdout);
  freeaddrinfo(results);
  close(socketfd);
  return exitStatus;
}
