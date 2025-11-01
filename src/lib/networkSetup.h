#ifndef NETWORKSETUP_H
#define NETWORKSETUP_H

#include "debug.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

void populateTCPHint(addrinfo *hints);

#endif