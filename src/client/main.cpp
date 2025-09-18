#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include "ip.h"
#include "debug.h"
#include "calc.h"
#include "netparser.h"
#include "macros.h"
#include <ctype.h>
#include "main.h"
#include <netinet/tcp.h>
#include <fcntl.h>

void validate_input_args(int argc, char *argv[])
{
  DEBUG_FUNCTION("client::main::validate_input_args(%d, ...)\n", argc);
  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<DNS|IPv4|IPv6>:<PORT>], got %d arguments\n", argc);
    exit(-1);
  }
  if (strstr(argv[1], "///") != NULL ){
    printf("Invalid format: %s.\n", argv[1]);
    exit(-1); 
  }
  if (strstr(argv[1], "://") == NULL) {
    printf("Invalid format: missing '://'\n");
    exit(-1);
  }
}

enum op stringToOp(char* input)
{
  DEBUG_FUNCTION("client::main::stringToOp(%s)", input);
  IF_ZERO(strcmp("add", input))
    return op::ADD;
  IF_ZERO(strcmp("sub", input))
    return op::SUB;
  IF_ZERO(strcmp("mul", input))
    return op::MUL;
  IF_ZERO(strcmp("div", input))
    return op::DIV;
  printf("ERROR: GOT UNEXPECTED OP COMMAND\n");
  DEBUG_FUNCTION("ERROR - %s is not a defined op", input);
  exit(EXIT_FAILURE);
}

void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort)
{
  DEBUG_FUNCTION("client::main::parseInputArgs(%s)\n", argv[1]);
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  getIPnPORT(argv[1], &destination, &destinationPort);
}


void populateTCPHint(addrinfo* hints)
{
  DEBUG_FUNCTION("client::main::populateTCPHint(%p)\n", hints);
  memset(hints, 0, sizeof(*hints));
  hints->ai_family = AF_UNSPEC;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_socktype = SOCK_STREAM;
}

int handleProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6])
{
  DEBUG_FUNCTION("client::main::handleProtocol(%d, %d, %s, %s)\n", \
                  foundProtocl, socketfd, pathstring, protocolstring );
  if (NOT foundProtocl)
  {
    char errorMessage[] = "ERROR\n";
    return send(socketfd, errorMessage, strlen(errorMessage), 0);
  }
  else
  {
    char successMessage[100];
    sprintf(successMessage, "%s %s 1.1 OK\n", pathstring, protocolstring);
    return send(socketfd, successMessage, strlen(successMessage), 0);
  }
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);
  int exitStatus = 0;
  char protocolstring[6], pathstring[7];
  char* destination, *destinationPort;
  parseInputArgs(argv, protocolstring, pathstring, destination, destinationPort);
  fd_set fdset;
  struct timeval tv;
  FD_ZERO(&fdset);
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  bool foundServer = false;
  bool timedOut = true;

  addrinfo hints;
  populateTCPHint(&hints);

  addrinfo* results;
  printf("Host %s, and port %s.\n",destination, destinationPort);
  int returnValue = getaddrinfo(
    destination,
    destinationPort,
    &hints,
    &results
  );
  if (results==NULL OR returnValue < 0)
  {
    printf("ERROR:");
    exitStatus = 1;
    goto freeMain;
  }

  struct addrinfo *rp;
  int socketfd;
  for (rp = results; rp != NULL; rp = rp->ai_next) {
    DEBUG_FUNCTION("Testing socket %p\n", rp);
    socketfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
    if (socketfd == -1)
    {
      perror("Problem with the socket: ");
      continue;
    }
    fcntl(socketfd, F_SETFL, O_NONBLOCK);
    connect(socketfd,  rp->ai_addr, rp->ai_addrlen);
    FD_SET(socketfd, &fdset);
    if (select(socketfd + 1, NULL, &fdset, NULL, &tv) == 1)
    {
      int so_error;
      socklen_t len = sizeof so_error;
      getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
      if (so_error == 0) {
          timedOut = false;
          DEBUG_FUNCTION("Testing connection %c\n", rp);
          fflush(stdout);
          foundServer = true;
          char expected_protocol[100];
          sprintf(expected_protocol, "%s %s 1.1\n", pathstring, protocolstring);
          char msg[1500];
          bool foundProtocol = getServerProtocols(socketfd, expected_protocol);
          if (NOT foundProtocol)
          {
            printf("ERROR: NO PROTOCOL FOUND (TIMEOUT)\n");
            DEBUG_FUNCTION("Failed to get a protocol from server after %d checks\n", 2000);
            exitStatus = 1;
            goto freeMain;
          }
          IF_NEGATIVE(handleProtocol(foundProtocol, socketfd, pathstring, protocolstring))
          {
            printf("ERROR: COULD NOT SEND PROTOCOL OK\n");
            DEBUG_FUNCTION("Could not send positive protocol to server %d\n", 0);
            exitStatus = 1;
            goto freeMain;
          }
          IF_NEGATIVE(getServerTask(socketfd, msg))
          {
            printf("ERROR: COULD NOT SEND TASK TO SERVER (TIMEOUT)\n");
            DEBUG_FUNCTION("Could not get task from server %d\n", 0);
            exitStatus = 1;
            goto freeMain;
          }
          int result = calculateServerTask(msg);
          IF_NEGATIVE(sendResultToServer(result, socketfd))
          {
            printf("ERROR: COULD NOT SEND RESULT BACK TO SERVER\n");
            DEBUG_FUNCTION("Could not send result back to server %d\n", 0);
            exitStatus = 1;
            goto freeMain;
          }
          getResultResponseBack(socketfd);
          break;
        }
    }
  }
  if (foundServer EQUALS false)
  {
    if (timedOut EQUALS true)
    {
      printf("ERROR: MESSAGE LOST (TIMEOUT)\n");
      DEBUG_FUNCTION("Found no server to connect to on ip %s.\n", destination);
      exitStatus = 1;
    }
    else
    {
      printf("ERROR: COULD NOT FIND A SERVER (TIMEOUT)\n");
      DEBUG_FUNCTION("Found no server to connect to on ip %s.\n", destination);
      exitStatus = 1;
    }
  }
  freeMain:
  freeaddrinfo(results);
  close(socketfd);
  free(destination);
  free(destinationPort);
  return exitStatus;
}

void getResultResponseBack(int socketfd)
{
  DEBUG_FUNCTION("client::main::getResultResponseBack(%d)\n", socketfd);
  char msg[1500];
  memset(msg, 0, 1500);
  static const int max_buffer_size = sizeof(msg)-1;
  recv(socketfd, &msg, max_buffer_size, 0);
  printf("Server Response - %s\n", msg);
}

int sendResultToServer(int result, int socketfd)
{
  DEBUG_FUNCTION("client::main::sendResultToServer(%d, %d)\n", result, socketfd);
  char resultMessage[100];
  memset(resultMessage, 0, 100);
  sprintf(resultMessage, "%d\n", result);
  return send(socketfd, resultMessage, strlen(resultMessage), 0);
}

int calculateServerTask(char* msg)
{
  DEBUG_FUNCTION("client::main::calculateServerTask(%s)\n", msg);
  char operation[10];
  int valueOne, valueTwo;
  IF_NEGATIVE(sscanf(msg, "%s %d %d", operation, &valueOne, &valueTwo))
  {
    printf("ERROR\n");
    DEBUG_FUNCTION("Could not properly read scaned values - %s", msg);
    exit(EXIT_FAILURE);
  }    
  DEBUG_FUNCTION("Split task into %s %d %d\n", operation, valueOne, valueTwo);
  int result;
  double temp;
  switch (stringToOp(operation))
  {
  case op::ADD:
      result = valueOne + valueTwo;
      break;
  case op::SUB:
      result = valueOne - valueTwo;
      break;
  case op::MUL:
      result = valueOne * valueTwo;
      break;
  case op::DIV:
      temp = valueOne / valueTwo;
      result = round(temp);
      break;
  default:
    break;
  }
  DEBUG_FUNCTION("Calculated %d\n", result);
  return result;
}

int getServerTask(int socketfd, char* msg)
{
  DEBUG_FUNCTION("client::main::getServerTask(%d %s)\n", socketfd, msg);
  static const int max_buffer_size = 1499;
  memset(msg, 0, 1500);
  int readSize = recv(socketfd, msg, max_buffer_size, 0);
  DEBUG_FUNCTION("client::main::getServerTask - Got %d bytes = %s", readSize, msg);
  return readSize;
}

bool getServerProtocols(int socketfd, char* expected_protocol)
{
  DEBUG_FUNCTION("client::main::getServerProtocols(%d, ...)\n", socketfd);
  bool foundProtocl = false;
  char msg[1500];
  static const int max_buffer_size = sizeof(msg)-1;
  int loop = 0;
  do
  {
    memset(msg, 0, 1500);
    DEBUG_FUNCTION("client::main::fromServer - Waiting %d\n", loop++);
    int readSize = recv(socketfd, &msg, max_buffer_size, 0);
    IF_NEGATIVE(readSize)
    return false;
    if (strcmp(msg, expected_protocol))
      foundProtocl = true;
    DEBUG_FUNCTION("client::main::fromServer - Received - %s", msg);
  } while (msg[0] != '\n' AND loop < 2000);
  DEBUG_FUNCTION("Got task %s", msg);
  fflush(stdout);
  return foundProtocl;
}
