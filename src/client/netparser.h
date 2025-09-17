#ifndef NETPARSER_H
#define NETPARSER_H

#include <stdlib.h>
#include <string.h>
#include "ip.h"


void getProtocol(char* input, char* protocol);
void getAPI(char* input, char* api);
void getIPnPORT(char* input, char** ip, char** port);

#endif