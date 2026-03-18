#include "src/client/udp.h"


bool getServerProtocols(int socketfd, calcProtocol* serverMessage) {
  DEBUG_FUNCTION("client::udp::getServerProtocols(%d, %p)\n", socketfd, serverMessage);
  calcMessage firstMessage;
  firstMessage.type = htons(22);
  firstMessage.message = htonl(0);
  firstMessage.protocol = htons(17);
  firstMessage.major_version = htons(1);
  firstMessage.minor_version = htons(1);
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
  }
  else if (bytesReceived == sizeof(calcMessage)) {
    return false;
  }
  else if (bytesReceived == sizeof(calcProtocol)) {
    serverMessage->type = ntohs(serverMessage->type);
    serverMessage->major_version = ntohs(serverMessage->major_version);
    serverMessage->minor_version = ntohs(serverMessage->minor_version);
    serverMessage->id = ntohl(serverMessage->id);
    serverMessage->arith = ntohl(serverMessage->arith);
    serverMessage->inValue1 = ntohl(serverMessage->inValue1);
    serverMessage->inValue2 = ntohl(serverMessage->inValue2);
    serverMessage->inResult = ntohl(serverMessage->inResult);
    return true;
  }
  else {
    return false;
  }
}

bool calculateTaskUDP(int socketfd, calcProtocol* serverProtocol, calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::calculateTaskUDP(%d, %p)\n", socketfd, serverProtocol);
  int result;
  switch(serverProtocol->arith) {
    case 1:
      result = serverProtocol->inValue1 +
                serverProtocol->inValue2;
      break;
    case 2:
      result = serverProtocol->inValue1 -
                serverProtocol->inValue2;
      break;
    case 3:
      result = serverProtocol->inValue1 *
              serverProtocol->inValue2;
      break;
    case 4:
      result = round((double)serverProtocol->inValue1 /
                (double)serverProtocol->inValue2);
      break;
    default:
      printf("ERROR: UNEXPECTED OPERATION\n");
      return false;
  }
  clientResponse->type = 2;
  clientResponse->major_version = 1;
  clientResponse->minor_version = 1;
  clientResponse->id = serverProtocol->id;
  clientResponse->arith = serverProtocol->arith;
  clientResponse->inValue1 = serverProtocol->inValue1;
  clientResponse->inValue2 = serverProtocol->inValue2;
  clientResponse->inResult = result;
  return true;
}

bool sendTaskResultsUDP(int socketfd, calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::udp::sendTaskResultsUDP(%d, %p)\n", socketfd, clientResponse);
  clientResponse->type = htons(clientResponse->type);
  clientResponse->major_version = htons(clientResponse->major_version);
  clientResponse->minor_version = htons(clientResponse->minor_version);
  clientResponse->id = htonl(clientResponse->id);
  clientResponse->arith = htonl(clientResponse->arith);
  clientResponse->inValue1 = htonl(clientResponse->inValue1);
  clientResponse->inValue2 = htonl(clientResponse->inValue2);
  clientResponse->inResult = htonl(clientResponse->inResult);
  ssize_t bytesSent = send(socketfd, clientResponse, sizeof(calcProtocol), 0);
  if (bytesSent < 0) {
    printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
    return false;
  }
  return true;
}


bool getResultResponseBackUDP(int socketfd, int expectedResult) {
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
  if (bytesReceived == sizeof(calcMessage)) {
    responseMessage.type = ntohs(responseMessage.type);
    responseMessage.message = ntohl(responseMessage.message);
    responseMessage.protocol = ntohs(responseMessage.protocol);
    responseMessage.major_version = ntohs(responseMessage.major_version);
    responseMessage.minor_version = ntohs(responseMessage.minor_version);
    if (responseMessage.message == 1) {
      printf("OK\n");
      return true;
    } else {
      printf("NOT OK\n");
      return false;
    }
  }
  else {
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