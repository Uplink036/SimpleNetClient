#ifndef SRC_CLIENT_ARGPARSE_H_
#define SRC_CLIENT_ARGPARSE_H_

#include <string.h>
#include "src/debug.h"
#include "src/client/netparser.h"
#include "src/lib/macros.h"

void parseInputArgs(
    char *argv[],
    char protocolstring[6],
    char pathstring[7],
    char **destination,
    char **destinationPort);
void validateInputArgs(int argc, char *argv[]);

#endif  // SRC_CLIENT_ARGPARSE_H_
