#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string.h>
#include "macros.h"
#include "debug.h"
#include "netparser.h"

void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort);
void validateInputArgs(int argc, char *argv[]);

#endif