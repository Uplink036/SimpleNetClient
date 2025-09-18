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
  printf("ERROR\n");
  DEBUG_FUNCTION("ERROR - %s is not a defined op", input);
  exit(EXIT_FAILURE);
}

void parseInputArgs(char *argv[], char protocolstring[6], char pathstring[7], char *&destination, char *&destinationPort)
{
  DEBUG_FUNCTION("client::main::parseInputArgs(%s)", argv[1]);
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  for (int i = 0; i < 7; i++)
  {
    pathstring[i] = toupper(pathstring[i]);
  }
  getIPnPORT(argv[1], &destination, &destinationPort);
}


void populateTCPHint(addrinfo* hints)
{
  DEBUG_FUNCTION("client::main::populateTCPHint(%p)", hints);
  memset(hints, 0, sizeof(*hints));
  hints->ai_family = AF_UNSPEC;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_socktype = SOCK_STREAM;
}

void handleProtocol(bool foundProtocl, int socketfd, char pathstring[7], char protocolstring[6])
{
  DEBUG_FUNCTION("client::main::handleProtocol(%d, %d, %s, %s)\n", \
                  foundProtocl, socketfd, pathstring, protocolstring );
  if (NOT foundProtocl)
  {
    char errorMessage[] = "ERROR\n";
    send(socketfd, errorMessage, strlen(errorMessage), 0);
    {
      return;
    };
  }
  else
  {
    char successMessage[100];
    sprintf(successMessage, "%s %s 1.1 OK\n", pathstring, protocolstring);
    send(socketfd, successMessage, strlen(successMessage), 0);
  }
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);
  
  char protocolstring[6], pathstring[7];
  char* destination, *destinationPort;
  parseInputArgs(argv, protocolstring, pathstring, destination, destinationPort);

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
  IF_NEGATIVE(returnValue)
  {
    printf("ERROR");
    return returnValue;
  }

  struct addrinfo *rp;
  int socketfd;
  for (rp = results; rp != NULL; rp = rp->ai_next) {
    socketfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
    if (socketfd == -1)
        continue;

    if (connect(socketfd, rp->ai_addr, rp->ai_addrlen) == 0)
    {
      char expected_protocol[100];
      sprintf(expected_protocol, "%s %s 1.1\n", pathstring, protocolstring);
      char msg[1500];
      bool foundProtocl = getServerProtocols(socketfd, expected_protocol);
      if (NOT foundProtocl)
        break;
      handleProtocol(foundProtocl, socketfd, pathstring, protocolstring);
      getServerTask(socketfd, msg);
      int result = calculateServerTask(msg);
      sendResultToServer(result, socketfd);
      getResultResponseBack(socketfd);
      break;
    }
  }
  freeaddrinfo(results);
  close(socketfd);
  free(destination);
  free(destinationPort);
  return 0;
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

void sendResultToServer(int result, int socketfd)
{
  DEBUG_FUNCTION("client::main::sendResultToServer(%d, %d)\n", result, socketfd);
  char resultMessage[100];
  memset(resultMessage, 0, 100);
  sprintf(resultMessage, "%d\n", result);
  send(socketfd, resultMessage, strlen(resultMessage), 0);
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

void getServerTask(int socketfd, char* msg)
{
  DEBUG_FUNCTION("client::main::getServerTask(%d %s)\n", socketfd, msg);
  static const int max_buffer_size = 1499;
  memset(msg, 0, 1500);
  int readSize = recv(socketfd, msg, max_buffer_size, 0);
  DEBUG_FUNCTION("client::main::getServerTask - Got %d bytes = %s", readSize, msg);
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
  } while (msg[0] != '\n');
  DEBUG_FUNCTION("Got task %s", msg);
  fflush(stdout);
  return foundProtocl;
}
