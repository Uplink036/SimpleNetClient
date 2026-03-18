#include "src/client/binary.h"


void buildProtocolRequest(calcMessage* msg) {
  msg->type = htons(22);
  msg->message = htonl(0);
  msg->protocol = htons(17);
  msg->major_version = htons(1);
  msg->minor_version = htons(1);
}

void decodeCalcProtocol(calcProtocol* proto) {
  proto->type = ntohs(proto->type);
  proto->major_version = ntohs(proto->major_version);
  proto->minor_version = ntohs(proto->minor_version);
  proto->id = ntohl(proto->id);
  proto->arith = ntohl(proto->arith);
  proto->inValue1 = ntohl(proto->inValue1);
  proto->inValue2 = ntohl(proto->inValue2);
  proto->inResult = ntohl(proto->inResult);
}

bool calculateBinaryTask(calcProtocol* serverProtocol, calcProtocol* clientResponse) {
  DEBUG_FUNCTION("client::binary::calculateBinaryTask(%p, %p)\n", serverProtocol, clientResponse);
  int result;
  char operation[5];
  switch (serverProtocol->arith) {
    case 1:
      result = serverProtocol->inValue1 + serverProtocol->inValue2;
      strcpy(operation, "add");
      break;
    case 2:
      result = serverProtocol->inValue1 - serverProtocol->inValue2;
      strcpy(operation, "sub");
      break;
    case 3:
      result = serverProtocol->inValue1 * serverProtocol->inValue2;
      strcpy(operation, "mul");
      break;
    case 4:
      result = round((double)serverProtocol->inValue1 /
                     (double)serverProtocol->inValue2);
      strcpy(operation, "div");
      break;
    default:
      printf("ERROR: UNEXPECTED OPERATION\n");
      return false;
  }
  printf("ASSIGNMENT: %s %d %d\n", operation,
         serverProtocol->inValue1, serverProtocol->inValue2);
  DEBUG_FUNCTION("Calculated %d\n", result);
  clientResponse->type = 2;
  clientResponse->major_version = 1;
  clientResponse->minor_version = 1;
  clientResponse->id = serverProtocol->id;
  clientResponse->arith = serverProtocol->arith;
  clientResponse->inValue1 = serverProtocol->inValue1;
  clientResponse->inValue2 = serverProtocol->inValue2;
  clientResponse->inResult = result;
  return true;
}

void encodeCalcProtocol(calcProtocol* proto) {
  proto->type = htons(proto->type);
  proto->major_version = htons(proto->major_version);
  proto->minor_version = htons(proto->minor_version);
  proto->id = htonl(proto->id);
  proto->arith = htonl(proto->arith);
  proto->inValue1 = htonl(proto->inValue1);
  proto->inValue2 = htonl(proto->inValue2);
  proto->inResult = htonl(proto->inResult);
}

void decodeCalcMessage(calcMessage* msg) {
  msg->type = ntohs(msg->type);
  msg->message = ntohl(msg->message);
  msg->protocol = ntohs(msg->protocol);
  msg->major_version = ntohs(msg->major_version);
  msg->minor_version = ntohs(msg->minor_version);
}
