#ifndef SRC_CLIENT_UDP_H_
#define SRC_CLIENT_UDP_H_

#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include "src/debug.h"
#include "src/lib/macros.h"
#include "src/lib/networkSetup.h"
#include "src/client/communication.h"
#include "src/client/text.h"
#include "src/client/binary.h"
#include <stdio.h>

static bool getServerProtocolsUDP(int socketfd, calcProtocol* serverMessage);
static bool calculateTaskUDP(int socketfd, calcProtocol* serverProtocol,
                             calcProtocol* clientResponse);
static bool sendTaskResultsUDP(int socketfd, calcProtocol* clientResponse);
static bool getResultResponseBackUDP(int socketfd, int expectedResult);
int connectUDP(char* destination, char* destinationPort,
               char pathstring[7], char protocolstring[6]);

#endif  // SRC_CLIENT_UDP_H_
