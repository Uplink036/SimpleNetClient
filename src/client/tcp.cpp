#include "src/client/tcp.h"

static bool waitForRead(int socketfd) {
  fd_set readSet;
  FD_ZERO(&readSet);
  FD_SET(socketfd, &readSet);
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  int selectResult = select(socketfd + 1, &readSet, NULL, NULL, &tv);
  if (selectResult <= 0) {
    printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
    return false;
  }
  return true;
}

static int getServerTask(int socketfd, char* msg) {
  DEBUG_FUNCTION("client::tcp::getServerTask(%d %s)\n", socketfd, msg);
  static const int max_buffer_size = 1499;
  memset(msg, 0, 1500);
  int readSize = recv(socketfd, msg, max_buffer_size, 0);
  DEBUG_FUNCTION("client::tcp::getServerTask - Got %d bytes = %s",
                 readSize, msg);
  return readSize;
}

static bool getServerProtocols(int socketfd, char* protocolRequest) {
  DEBUG_FUNCTION("client::tcp::getServerProtocols(%d, ...)\n", socketfd);
  bool foundProtocl = false;
  char taskMessage[1500];
  static const int max_buffer_size = sizeof(taskMessage)-1;
  int loop = 0;
  do {
    memset(taskMessage, 0, 1500);
    DEBUG_FUNCTION("client::tcp::fromServer - Waiting %d\n", loop++);
    if (waitForRead(socketfd)) {
      int readSize = recv(socketfd, &taskMessage, max_buffer_size, 0);
      DEBUG_FUNCTION("client::tcp::fromServer - Received %d bytes = %s",
                     readSize, taskMessage);
      IF_NEGATIVE(readSize)
        return false;
      DEBUG_FUNCTION("client::tcp::fromServer - Looking for - %s",
                     protocolRequest);
      if (strstr(taskMessage, protocolRequest) NOTEQUALS NULL) {
        foundProtocl = true;
        break;
      }
    } else {
      break;
    }
  } while (taskMessage[0] != '\n' AND loop < 2000);
  DEBUG_FUNCTION("Got task %s ", taskMessage);
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
  char responseMessage[1500];
  memset(responseMessage, 0, 1500);
  static const int max_buffer_size = sizeof(responseMessage)-1;
  recv(socketfd, &responseMessage, max_buffer_size, 0);
  if (strcmp(responseMessage, "OK\n") == 0)
    printf("OK (myresult=%d)\n", result);
  else
    printf("Fail myresult=%d, server response %s", result, responseMessage);
}

static bool handleTextTask(int socketfd) {
    char msg[1500];
  IF_NEGATIVE(getServerTask(socketfd, msg)) {
    printf("ERROR: COULD NOT SEND TASK TO SERVER (TIMEOUT)\n");
    DEBUG_FUNCTION("Could not get task from server %d\n", 0);
    return false;
  }
  int result = calculateTextTask(msg);
  IF_NEGATIVE(sendResultToServer(result, socketfd)) {
    printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
    DEBUG_FUNCTION("Could not send result back to server %d\n", 0);
    return false;
  }
  getResultResponseBack(socketfd, result);
  return true;
}

static bool handleBinaryTask(int socketfd) {
  calcProtocol serverMessage;
  ssize_t bytesReceived = recv(socketfd, &serverMessage, sizeof(calcProtocol), 0);
  if (bytesReceived < 0) {
    printf("ERROR: COULD NOT GET BINARY TASK FROM SERVER\n");
      DEBUG_FUNCTION("Could not get binary task from server %d\n", 0);
      return false;
  } else if (bytesReceived == 0) {
    printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
    DEBUG_FUNCTION("Could not get binary task from server %d\n", 0);
    return false;
  }
  decodeCalcProtocol(&serverMessage);
  calcProtocol clientResponse;
  calculateBinaryTask(&serverMessage, &clientResponse);
  int result = clientResponse.inResult;
  encodeCalcProtocol(&clientResponse);
  ssize_t bytesSent = send(socketfd, &clientResponse, sizeof(clientResponse), 0);
  if (bytesSent != (ssize_t)sizeof(clientResponse)) {
    printf("ERROR: COULD NOT SEND BINARY RESULT TO SERVER\n");
    return false;
  }

  calcMessage responseMessage;
  memset(&responseMessage, 0, sizeof(responseMessage));
  bytesReceived = recv(socketfd, &responseMessage, sizeof(responseMessage), 0);
  if (bytesReceived != (ssize_t)sizeof(responseMessage)) {
    printf("ERROR: COULD NOT GET BINARY RESPONSE BACK FROM SERVER\n");
    return false;
  }
  else if (bytesReceived == 0) {
    printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
    DEBUG_FUNCTION("Could not get binary task from server %d\n", 0);
    return false;
  }
  decodeCalcMessage(&responseMessage);
  if (responseMessage.message == 1) {
    printf("OK (myresult=%d)\n", result);
    return true;
  }
  printf("NOT OK\n");
  return false;
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
    printf("ERROR: COULD NOT GET ADDRESS INFO FOR DESTINATION\n");
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
        char protocolRequest[100];
        snprintf(protocolRequest, sizeof(protocolRequest),
                 "%s %s 1.1\n", pathstring, protocolstring);
        bool foundProtocol = getServerProtocols(socketfd, protocolRequest);
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
        bool handledTask = false;
        if (strcmp(pathstring, "TEXT") == 0)
          handledTask = handleTextTask(socketfd);
        else if (strcmp(pathstring, "BINARY") == 0)
          handledTask = handleBinaryTask(socketfd);
        else {
          printf("ERROR: UNSUPPORTED APPLICATION PROTOCOL %s\n", pathstring);
          exitStatus = 1;
          goto freeTCP;
        }
        if (NOT handledTask) {
          exitStatus = 1;
          goto freeTCP;
        }
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
