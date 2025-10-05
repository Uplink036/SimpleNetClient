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
#include <ctype.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "netparser.h"
#include "macros.h"
#include "communication.h"
#include "argParse.h"
#include "networkSetup.h"

int handleProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6]);
int calculateServerTask(char* msg);

#endif