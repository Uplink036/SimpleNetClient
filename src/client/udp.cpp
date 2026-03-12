#include "src/client/udp.h"

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