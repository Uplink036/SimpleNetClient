#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "macros.h"
#include "protocol.h"
#include "task.h"
#include "networkSetup.h"

#define MAX_PENDING_CONNECTIONS 2
#define MAX_MESSGE_SIZE 1500

void listenOnPort(int socket_fd, addrinfo* serverIP);
int bindPort(char* desiredPort, addrinfo** selectedIP);

#endif