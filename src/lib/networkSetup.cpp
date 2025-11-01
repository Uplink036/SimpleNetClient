#include "networkSetup.h"

void populateTCPHint(addrinfo* hints)
{
  DEBUG_FUNCTION("client::main::populateTCPHint(%p)\n", hints);
  memset(hints, 0, sizeof(*hints));
  hints->ai_family = AF_UNSPEC;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_socktype = SOCK_STREAM;
}
