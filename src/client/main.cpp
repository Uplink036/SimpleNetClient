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
  IF_ZERO(strcmp("add", input))
    return op::ADD;
  IF_ZERO(strcmp("sub", input))
    return op::SUB;
  IF_ZERO(strcmp("mul", input))
    return op::MUL;
  IF_ZERO(strcmp("div", input))
    return op::DIV;
  printf("ERROR - %s is not a defined op", input);
  exit(-1);
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);
  
  char protocolstring[6], pathstring[7];
  getProtocol(argv[1], protocolstring);
  getAPI(argv[1], pathstring);
  for (int i = 0; i < 7; i++)
  {
    pathstring[i] = toupper(pathstring[i]);
  }
  char* destination;
  char* destinationPort; 
  getIPnPORT(argv[1], &destination, &destinationPort);

  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* results;

  DEBUG_FUNCTION("Host %s, and port %s.\n",destination, destinationPort);
  int returnValue = getaddrinfo(
    destination,
    destinationPort,
    &hints,
    &results
  );
  if (returnValue < 0)
  {
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
      bool foundProtocl = false;
      sprintf(expected_protocol, "%s %s 1.1\n", pathstring, protocolstring);
      char msg[1500];
      static const int max_buffer_size = sizeof(msg)-1;
      do
      {
        memset(msg, 0, 1500);
        int readSize=recv(socketfd,&msg,max_buffer_size,0);
        IF_NEGATIVE(readSize)
          return false;
        if (strcmp(msg, expected_protocol))
          foundProtocl = true;
        DEBUG_FUNCTION("client::main::fromServer - Received - %s", msg);
      } while(msg[0] != '\n');

      if (NOT foundProtocl)
      {
        char errorMessage[] = "ERROR\n";
        send(socketfd, errorMessage, strlen(errorMessage), 0);
        break;
      }
      else 
      {
        char successMessage[100];
        sprintf(successMessage, "%s %s 1.1 OK\n", pathstring, protocolstring);
        send(socketfd, successMessage, strlen(successMessage), 0);
      }
      DEBUG_FUNCTION("Waiting for task %d\n", foundProtocl);
      memset(msg, 0, 1500);
      int readSize=recv(socketfd,&msg,max_buffer_size,0);
      DEBUG_FUNCTION("Got task %s", msg);
      char operation[10];
      int valueOne, valueTwo;
      sscanf(msg, "%s %d %d", operation, &valueOne, &valueTwo);    
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
      char resultMessage[100];
      memset(resultMessage, 0, 100);
      sprintf(resultMessage, "%d\n", result);
      send(socketfd, resultMessage, strlen(resultMessage),0);
      memset(msg, 0, 1500);
      recv(socketfd, &msg, max_buffer_size, 0);
      printf("Server Response - %s\n", msg);
      break;
    }
  }
  freeaddrinfo(results);
  close(socketfd);
  free(destination);
  free(destinationPort);
  return 0;
}
