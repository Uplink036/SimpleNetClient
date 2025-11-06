#ifndef SRC_CLIENT_COMMUNICATION_H_
#define SRC_CLIENT_COMMUNICATION_H_


#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <fcntl.h>
#include "src/lib/ip.h"
#include "src/debug.h"
#include "src/lib/calc.h"
#include "src/client/netparser.h"
#include "src/lib/macros.h"


int getServerTask(int socketfd, char* msg);
bool getServerProtocols(int socketfd, char* expected_protocol, fd_set* fdset,
                        timeval* tv);
int sendClientProtocol(bool foundProtocl, int socketfd, char pathstring[7],
                       char protocolstring[6]);
int sendResultToServer(int result, int socketfd);
void getResultResponseBack(int socketfd, int result);

#endif  // SRC_CLIENT_COMMUNICATION_H_
