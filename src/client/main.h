#ifndef SRC_CLIENT_MAIN_H_
#define SRC_CLIENT_MAIN_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <cmath>
#include <ctype.h>
#include "src/debug.h"
#include "src/lib/ip.h"
#include "src/lib/calc.h"
#include "src/client/netparser.h"
#include "src/lib/macros.h"
#include "src/client/communication.h"
#include "src/client/argParse.h"
#include "src/lib/networkSetup.h"

int sendClientProtocol(bool foundProtocl, int socketfd, char pathstring[7],
                       char protocolstring[6]);
int calculateServerTask(char* msg);

#endif  // SRC_CLIENT_MAIN_H_
