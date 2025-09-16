#include <stdio.h>
#include <server.h>

void validate_input_args(int argc, char *argv[])
{
  DEBUG_FUNCTION("server::main::validate_input_args(&d, ...)\n", argc);
  if (argc != 2)
  {
    printf("Unexpected amount of inputs, expected [<PROGRAM>] [<PORT>], got %d arguments\n", argc);
    fflush(stdout);
    exit(-1);
  }
  if (!validPORT(argv[1]))
  {
    printf("Not a valid port, expected a value between 0-65535, got %s\n", argv[1]);
    fflush(stdout);
    exit(-1);
  }
}

int main(int argc, char *argv[]){
  validate_input_args(argc, argv);

  int socket_fd;
  addrinfo* serverIP;
  socket_fd = bindPort(argv[1], &serverIP);
  IF_NEGATIVE(socket_fd)
  {
    fprintf(stderr, "Could not bind to specified port\n");
    return socket_fd;
  }

  printf("server: Waiting for connections...\n");
	listenOnPort(socket_fd, serverIP);
  close(socket_fd);
  freeaddrinfo(serverIP);
  return 0;
}