#ifndef IP_H
#define IP_H

#include <stdlib.h>
#include <string.h>
#include "debug.h"

int splitIPPortFromString(char* string, char**ip, char** port);
bool validPORT(char* port);
bool validIP(char* ip);
/*
Given a string, it will split it into ip and port, which we be malloced for this reason.
If it fails, it will return a negativ value, otherwise it will return a positive one.
*/

#endif