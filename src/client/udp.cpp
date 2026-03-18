#include "src/client/udp.h"
#include "src/client/binary.h"


static bool getServerProtocols(int socketfd, calcProtocol* serverMessage) {
  DEBUG_FUNCTION("client::udp::getServerProtocols(%d, %p)\n", socketfd, serverMessage);
  calcMessage firstMessage;
  buildProtocolRequest(&firstMessage);
  ssize_t bytesSent = send(socketfd, &firstMessage, sizeof(calcMessage), 0);
  if (bytesSent < 0) {
    perror("ERROR: COULD NOT SEND PROTOCOL REQUEST TO SERVER\n");
    return false;
  }

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

static bool calculateTaskUDP(int socketfd, calcProtocol* serverProtocol,
                             calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::calculateTaskUDP(%d, %p)\n", socketfd, serverProtocol);
  return calculateTask(serverProtocol, clientResponse);
}

static bool sendTaskResultsUDP(int socketfd, calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::sendTaskResultsUDP(%d, %p)\n", socketfd, clientResponse);
  encodeCalcProtocol(clientResponse);
  ssize_t bytesSent = send(socketfd, clientResponse, sizeof(calcProtocol), 0);
  if (bytesSent < 0) {
    printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
    return false;
  }
  return true;
}

static bool getResultResponseBackUDP(int socketfd, int expectedResult) {
  DEBUG_FUNCTION("client::udp::getResultResponseBackUDP(%d, %d)\n", socketfd, expectedResult);
  calcMessage responseMessage;
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
  ssize_t bytesReceived = recv(socketfd, &responseMessage, sizeof(calcMessage), 0);
  if (bytesReceived == (ssize_t)sizeof(calcMessage)) {
    decodeCalcMessage(&responseMessage);
    if (responseMessage.message == 1) {
      printf("OK\n");
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
  bool foundServer = false;
  bool timedOut = true;

  addrinfo hints;
  populateUDPHint(&hints);

  addrinfo* results;
  printf("Host %s, and port %s.\n", destination, destinationPort);
  int returnValue = getaddrinfo(destination, destinationPort, &hints,
                                &results);
  if (results == NULL OR returnValue < 0) {
    printf("ERROR:");
    exitStatus = 1;
    goto freeUDP;
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
        calcProtocol serverMessage;
        memset(&serverMessage, 0, sizeof(serverMessage));
        bool foundProtocol = getServerProtocols(socketfd, &serverMessage);
        if (NOT foundProtocol) {
          printf("ERROR: NOT OK (ERROR WRONG SIZE OR INCORRECT PROTOCOL)\n");
          DEBUG_FUNCTION("Failed to get a protocol from server after");
          exitStatus = 1;
          goto freeUDP;
        }
        calcProtocol clientResponse;
        bool calculatedTask = calculateTaskUDP(socketfd, &serverMessage, &clientResponse);
        if (NOT calculatedTask) {
          printf("ERROR: NOT OK (ERROR CALCULATING TASK)\n");
          DEBUG_FUNCTION("Failed to calculate task from server\n");
          exitStatus = 1;
          goto freeUDP;
        }
        bool sentResults = sendTaskResultsUDP(socketfd, &clientResponse);
        if (NOT sentResults) {
          printf("ERROR: NOT OK (ERROR SENDING RESULTS)\n");
          DEBUG_FUNCTION("Failed to send results back to server\n");
          exitStatus = 1;
          goto freeUDP;
        }
        bool gotResponseBack = getResultResponseBackUDP(socketfd, serverMessage.inResult);
        if (NOT gotResponseBack) {
          printf("ERROR: NOT OK (ERROR GETTING RESPONSE BACK FROM SERVER)\n");
          DEBUG_FUNCTION("Failed to get response back from server\n");
          exitStatus = 1;
          goto freeUDP;
        }
      }
    }
  }
  freeUDP:
  fflush(stdout);
  close(socketfd);
  return exitStatus;
}