#ifndef SRC_LIB_NETWORKSETUP_H_
#define SRC_LIB_NETWORKSETUP_H_

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include "src/debug.h"

void populateTCPHint(addrinfo *hints);

#endif  // SRC_LIB_NETWORKSETUP_H_
