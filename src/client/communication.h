#ifndef COMMUNICATION_H
#define COMMUNICATION_H


#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <fcntl.h>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "netparser.h"
#include "macros.h"


int getServerTask(int socketfd, char* msg);
bool getServerProtocols(int socketfd, char* expected_protocol, fd_set* fdset, timeval* tv);
int sendClientProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6]);
int sendResultToServer(int result, int socketfd);
void getResultResponseBack(int socketfd, int result);

#endif