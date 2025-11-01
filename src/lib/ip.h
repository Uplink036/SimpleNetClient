#ifndef SRC_LIB_IP_H_
#define SRC_LIB_IP_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include "src/debug.h"

int splitIPPortFromString(char* string, char**ip, char** port);
bool validPORT(char* port);
bool validIP(char* ip);

#endif  // SRC_LIB_IP_H_
