#include "src/client/udp.h"

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

static bool handleTextTask(int socketfd, char pathstring[7],
                           char protocolstring[6]) {
  DEBUG_FUNCTION("client::udp::handleTextTask(%d, %s, %s)\n", socketfd,
                 pathstring, protocolstring);

  char protocolRequest[100];
  snprintf(protocolRequest, sizeof(protocolRequest), "%s %s 1.1\n",
           pathstring, protocolstring);
  ssize_t bytesSent = send(socketfd, protocolRequest, strlen(protocolRequest), 0);
  if (bytesSent != (ssize_t)strlen(protocolRequest)) {
    printf("ERROR: COULD NOT SEND TEXT PROTOCOL REQUEST TO SERVER\n");
    return false;
  }

  char taskMessage[1500];
  memset(taskMessage, 0, sizeof(taskMessage));
  if (NOT waitForRead(socketfd))
    return false;
  ssize_t bytesReceived = recv(socketfd, taskMessage, sizeof(taskMessage)-1, 0);
  if (bytesReceived <= 0) {
    printf("ERROR: COULD NOT GET TEXT TASK FROM SERVER\n");§
    return false;
  }
  taskMessage[bytesReceived] = '\0';

  int result = calculateTextTask(taskMessage);
  char resultMessage[100];
  memset(resultMessage, 0, sizeof(resultMessage));
  formatTextResult(result, resultMessage, sizeof(resultMessage));
  bytesSent = send(socketfd, resultMessage, strlen(resultMessage), 0);
  if (bytesSent != (ssize_t)strlen(resultMessage)) {
    printf("ERROR: COULD NOT SEND TEXT RESULT BACK TO SERVER\n");
    return false;
  }

  char responseMessage[1500];
  memset(responseMessage, 0, sizeof(responseMessage));
  if (NOT waitForRead(socketfd))
    return false;
  bytesReceived = recv(socketfd, responseMessage, sizeof(responseMessage)-1, 0);
  if (bytesReceived <= 0) {
    printf("ERROR: COULD NOT GET RESPONSE BACK FROM SERVER\n");
    return false;
  }
  responseMessage[bytesReceived] = '\0';

  if (strcmp(responseMessage, "OK\n") == 0) {
    printf("OK (myresult=%d)\n", result);
    return true;
  }
  printf("NOT OK\n");
  return false;
}


static bool getServerProtocols(int socketfd, calcProtocol* serverMessage) {
  DEBUG_FUNCTION("client::udp::getServerProtocols(%d, %p)\n", socketfd, serverMessage);
  calcMessage firstMessage;
  buildProtocolRequest(&firstMessage);
  ssize_t bytesSent = send(socketfd, &firstMessage, sizeof(calcMessage), 0);
  if (bytesSent < 0) {
    perror("ERROR: COULD NOT SEND PROTOCOL REQUEST TO SERVER\n");
    return false;
  }

  if (NOT waitForRead(socketfd)) {
    return false;
  }

  ssize_t bytesReceived = recv(socketfd, serverMessage, sizeof(calcProtocol), 0);
  if (bytesReceived < 0) {
    perror("ERROR: PROTOCOL NOT RECEIVED");
    return false;
  } else if (bytesReceived == (ssize_t)sizeof(calcMessage)) {
    return false;
  } else if (bytesReceived == (ssize_t)sizeof(calcProtocol)) {
    decodeCalcProtocol(serverMessage);
    return true;
  } else {
    return false;
  }
}

static bool calculateTask(int socketfd, calcProtocol* serverProtocol,
                          calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::calculateTask(%d, %p)\n", socketfd, serverProtocol);
  return calculateBinaryTask(serverProtocol, clientResponse);
}

static bool sendTaskResults(int socketfd, calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::sendTaskResults(%d, %p)\n", socketfd, clientResponse);
  encodeCalcProtocol(clientResponse);
  ssize_t bytesSent = send(socketfd, clientResponse, sizeof(calcProtocol), 0);
  if (bytesSent < 0) {
    printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
    return false;
  }
  return true;
}

static bool getResultResponseBack(int socketfd, int result) {
  DEBUG_FUNCTION("client::udp::getResultResponseBack(%d, %d)\n", socketfd, result);
  calcMessage responseMessage;
  if (NOT waitForRead(socketfd)) {
    return false;
  }
  ssize_t bytesReceived = recv(socketfd, &responseMessage, sizeof(calcMessage), 0);
  if (bytesReceived == (ssize_t)sizeof(calcMessage)) {
    decodeCalcMessage(&responseMessage);
    if (responseMessage.message == 1) {
      printf("OK (myresult=%d)\n", result);
      return true;
    } else {
      printf("NOT OK\n");
      return false;
    }
  } else {
    printf("ERROR: COULD NOT GET RESPONSE BACK FROM SERVER (INCORRECT SIZE)\n");
    return false;
  }
}

static bool handleBinaryTask(int socketfd) {
  DEBUG_FUNCTION("client::udp::handleBinaryTask(%d)\n", socketfd);
  calcProtocol serverMessage;
  memset(&serverMessage, 0, sizeof(serverMessage));
  bool foundProtocol = getServerProtocols(socketfd, &serverMessage);
  if (NOT foundProtocol) {
    printf("ERROR: NOT OK (ERROR WRONG SIZE OR INCORRECT PROTOCOL)\n");
    DEBUG_FUNCTION("Failed to get a protocol from server after");
    return false;
  }

  calcProtocol clientResponse;
  bool calculatedTask = calculateTask(socketfd, &serverMessage, &clientResponse);
  if (NOT calculatedTask) {
    printf("ERROR: NOT OK (ERROR CALCULATING TASK)\n");
    DEBUG_FUNCTION("Failed to calculate task from server\n");
    return false;
  }

  int result = clientResponse.inResult;
  bool sentResults = sendTaskResults(socketfd, &clientResponse);
  if (NOT sentResults) {
    printf("ERROR: NOT OK (ERROR SENDING RESULTS)\n");
    DEBUG_FUNCTION("Failed to send results back to server\n");
    return false;
  }

  bool gotResponseBack = getResultResponseBack(socketfd, result);
  if (NOT gotResponseBack) {
    printf("ERROR: NOT OK (ERROR GETTING RESPONSE BACK FROM SERVER)\n");
    DEBUG_FUNCTION("Failed to get response back from server\n");
    return false;
  }
  return true;
}

int connectUDP(char* destination, char* destinationPort,
                char pathstring[7], char protocolstring[6]){
  DEBUG_FUNCTION("client::udp::connectUDP(%s, %s, %s, %s)\n",
    destination, destinationPort, pathstring, protocolstring);
  int exitStatus = 0;
  fd_set fdset;
  struct timeval tv;
  FD_ZERO(&fdset);
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  bool timedOut = true;

  addrinfo hints;
  populateUDPHint(&hints);

  int socketfd = -1;
  addrinfo* results = NULL;
  printf("Host %s, and port %s.\n", destination, destinationPort);
  int returnValue = getaddrinfo(destination, destinationPort, &hints,
                                &results);
  if (results == NULL OR returnValue < 0) {
    printf("ERROR:");
    exitStatus = 1;
    goto freeUDP;
  }
  struct addrinfo *rp;
  for (rp = results; rp != NULL; rp = rp->ai_next) {
    DEBUG_FUNCTION("Testing socket %p\n", rp);
    socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (socketfd == -1) {
      perror("Problem with the socket: ");
      continue;
    }
    fcntl(socketfd, F_SETFL, O_NONBLOCK);
    connect(socketfd, rp->ai_addr, rp->ai_addrlen);
    FD_SET(socketfd, &fdset);
    if (select(socketfd + 1, NULL, &fdset, NULL, &tv) == 1) {
      int so_error;
      socklen_t len = sizeof so_error;
      getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
      if (so_error == 0) {
        timedOut = false;
        DEBUG_FUNCTION("Testing connection %p\n", rp);
        fflush(stdout);
        if (strcmp(pathstring, "TEXT") == 0) {
          bool handledTask = handleTextTask(socketfd, pathstring, protocolstring);
          if (NOT handledTask) {
            exitStatus = 1;
            goto freeUDP;
          }
        } else if (strcmp(pathstring, "BINARY") == 0) {
          bool handledTask = handleBinaryTask(socketfd);
          if (NOT handledTask) {
            exitStatus = 1;
            goto freeUDP;
          }
        } else {
          printf("ERROR: UNSUPPORTED APPLICATION PROTOCOL %s\n", pathstring);
          exitStatus = 1;
          goto freeUDP;
        }
        break;
      }
    }
  }
  freeUDP:
  fflush(stdout);
  if (results != NULL)
    freeaddrinfo(results);
  if (socketfd >= 0)
    close(socketfd);
  if (timedOut EQUALS true AND exitStatus EQUALS 0) {
    printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
    exitStatus = 1;
  }
  return exitStatus;
}