#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "src/lib/ip.h"
#include "src/debug.h"
#include "src/lib/calc.h"
#include "src/lib/macros.h"
#include "src/server/protocol.h"
#include "src/server/task.h"
#include "src/lib/networkSetup.h"

#define MAX_PENDING_CONNECTIONS 2
#define MAX_MESSGE_SIZE 1500

void listenOnPort(int socket_fd, addrinfo* serverIP);
int bindPort(char* desiredPort, addrinfo** selectedIP);

#endif  // SRC_SERVER_SERVER_H_
