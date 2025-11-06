#ifndef SRC_CLIENT_NETPARSER_H_
#define SRC_CLIENT_NETPARSER_H_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "src/lib/ip.h"




void getProtocol(char* input, char* protocol);
void getAPI(char* input, char* api);
void getIPnPORT(char* input, char** ip, char** port);

#endif  // SRC_CLIENT_NETPARSER_H_
