#include "main.h"

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
  validateInputArgs(argc, argv);
  int exitStatus = 0;
  char protocolstring[6], pathstring[7];
  char* destination, *destinationPort;
  parseInputArgs(argv, protocolstring, pathstring, destination, destinationPort);
  fd_set fdset;
  struct timeval tv;
  FD_ZERO(&fdset);
  tv.tv_sec = 2;
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
    int flags = fcntl(socketfd, F_GETFL);
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
          bool foundProtocol = getServerProtocols(socketfd, expected_protocol, &fdset, &tv);
          if (NOT foundProtocol)
          {
            printf("ERROR: NO PROTOCOL FOUND (TIMEOUT)\n");
            DEBUG_FUNCTION("Failed to get a protocol from server after %d checks\n", 2000);
            exitStatus = 1;
            goto freeMain;
          }
          fcntl(socketfd, F_SETFL, flags & ~O_NONBLOCK);
          IF_NEGATIVE(handleProtocol(foundProtocol, socketfd, pathstring, protocolstring))
          {
            printf("ERROR: COULD NOT SEND PROTOCOL OK\n");
            DEBUG_FUNCTION("Could not send positive protocol to server %d\n", 0);
            exitStatus = 1;
            goto freeMain;
          }
          char msg[1500];
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
          getResultResponseBack(socketfd, result);
          break;
        }
    }
    else
      break;
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
  fflush(stdout);
  freeaddrinfo(results);
  close(socketfd);
  free(destination);
  free(destinationPort);
  return exitStatus;
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
  printf("ASSIGNMENT: %s %d %d\n", operation, valueOne, valueTwo);
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
