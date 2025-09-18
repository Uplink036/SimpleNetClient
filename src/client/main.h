#ifndef MAIN_H
#define MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "netparser.h"
#include "macros.h"
#include <ctype.h>
#include <netinet/tcp.h>
#include <fcntl.h>


void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort);
void populateTCPHint(addrinfo *hints);
int handleProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6]);
void getResultResponseBack(int socketfd, int result);
int sendResultToServer(int result, int socketfd);
int getServerTask(int socketfd, char* msg);
bool getServerProtocols(int socketfd, char* expected_protocol, fd_set* fdset, timeval* tv);
int calculateServerTask(char* msg);

#endif