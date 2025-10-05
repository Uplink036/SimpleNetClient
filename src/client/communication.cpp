#include "communication.h"

int getServerTask(int socketfd, char* msg)
{
  DEBUG_FUNCTION("client::main::getServerTask(%d %s)\n", socketfd, msg);
  static const int max_buffer_size = 1499;
  memset(msg, 0, 1500);
  int readSize = recv(socketfd, msg, max_buffer_size, 0);
  DEBUG_FUNCTION("client::main::getServerTask - Got %d bytes = %s", readSize, msg);
  return readSize;
}

bool getServerProtocols(int socketfd, char* expected_protocol, fd_set* fdset, timeval* tv)
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
    if (select(socketfd + 1, fdset, NULL, NULL, tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) 
        {
          int readSize = recv(socketfd, &msg, max_buffer_size, 0);
          DEBUG_FUNCTION("client::main::fromServer - Received %d bytes = %s", readSize, msg);
          IF_NEGATIVE(readSize)
            return false;
          DEBUG_FUNCTION("client::main::fromServer - Looking for - %s", expected_protocol);
          if (strstr(msg, expected_protocol) NOTEQUALS NULL)
          {
            foundProtocl = true;
            break;
          }
        }
    }
    else
      break;
  } while (msg[0] != '\n' AND loop < 2000);
  DEBUG_FUNCTION("Got task %s ", msg);
  fflush(stdout);
  return foundProtocl;
}

int sendResultToServer(int result, int socketfd)
{
  DEBUG_FUNCTION("client::main::sendResultToServer(%d, %d)\n", result, socketfd);
  char resultMessage[100];
  memset(resultMessage, 0, 100);
  sprintf(resultMessage, "%d\n", result);
  return send(socketfd, resultMessage, strlen(resultMessage), 0);
}

void getResultResponseBack(int socketfd, int result)
{
  DEBUG_FUNCTION("client::main::getResultResponseBack(%d)\n", socketfd);
  char msg[1500];
  memset(msg, 0, 1500);
  static const int max_buffer_size = sizeof(msg)-1;
  recv(socketfd, &msg, max_buffer_size, 0);
  if (strcmp(msg, "OK\n") == 0)
    printf("OK (myresult=%d)\n", result);
  else
    printf("Fail myresult=%d, server response %s", result, msg);
}