#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ip.h"
#include "debug.h"
#include "macros.h"

#define MAX_MESSGE_SIZE 1500

extern const char* SERVER_PROTOCOLS[];

enum CommunicationProtocl
{
  TEXT,
  BINARY
};

enum InternetProtocl
{
  TCP,
  UDP
};

struct ApplicationProtocl
{
  CommunicationProtocl com;
  InternetProtocl proc;
};
typedef struct ApplicationProtocl ApplicationProtocl;

bool sendServerProtocol(int client_fd);
bool getClientProtocol(int client_fd, ApplicationProtocl* clientProtocol);

#endif