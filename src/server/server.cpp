#include "src/server/server.h"

void listenOnPort(int socket_fd, addrinfo* serverIP) {
  DEBUG_FUNCTION("server::server::listenOnPort(%d, %p)\n", socket_fd, serverIP);
  int childCnt = 0;
  socklen_t socket_in_size;
  struct sockaddr_storage their_addr;
  char client[INET6_ADDRSTRLEN];
  int new_fd;
  while (true) {
    socket_in_size = sizeof(their_addr);
    new_fd = accept(socket_fd, (struct sockaddr *)&their_addr,
                    &socket_in_size);
    IF_NEGATIVE(new_fd) {
      perror("accept");
      continue;
    }
    getnameinfo(serverIP->ai_addr, serverIP->ai_addrlen, client,
                sizeof(client), nullptr, 0, 0);
    printf("server: Connection %d from %s\n", childCnt, client);

    if (sendServerProtocol(new_fd) EQUALS false)
      continue;
    ApplicationProtocl clientProtocol;
    if (getClientProtocol(new_fd, (&clientProtocol)) EQUALS false)
      continue;

    if (clientTask(new_fd) EQUALS false)
      continue;
    close(new_fd);
    printf("Socket closed()\n");
  }
}

int bindPort(char* desiredPort, addrinfo** selectedIP) {
  DEBUG_FUNCTION("server::server::bindPort(%s, %p)\n", desiredPort, selectedIP);

  addrinfo hints;
  addrinfo* avaliableIPs;
  int returnValue;

  populateTCPHint(&hints);
  returnValue = getaddrinfo(NULL, desiredPort, &hints, &avaliableIPs);
  IF_NOT_ZERO(returnValue) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(returnValue));
    return -1;
  }

  int socket_fd;
  addrinfo* p;
  for (p = avaliableIPs; p != NULL; p = p->ai_next) {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    IF_NEGATIVE(socket_fd) {
        perror("server: socket\n");
        continue;
      }

    const int yes = 1;
    IF_NOT_ZERO(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                           sizeof(int))) {
      perror("setsockopt");
      return -1;
    }

    IF_NEGATIVE(bind(socket_fd, p->ai_addr, p->ai_addrlen)) {
      close(socket_fd);
      perror("server: bind");
      continue;
    }
    break;
  }
  freeaddrinfo(avaliableIPs);
  *selectedIP = p;
  IF_NULL(p)  {
    fprintf(stderr, "server: Failed to bind\n");
    return -1;
  }

  IF_NEGATIVE(listen(socket_fd, MAX_PENDING_CONNECTIONS)) {
    perror("listen");
    return -1;
  }
  return socket_fd;
}
