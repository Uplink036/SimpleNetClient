#ifndef SRC_CLIENT_BINARY_H_
#define SRC_CLIENT_BINARY_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include <cstdio>
#include "src/debug.h"
#include "src/lib/macros.h"
#include "src/lib/messageProtocol.h"

void buildProtocolRequest(calcMessage* msg);
void decodeCalcProtocol(calcProtocol* proto);
bool calculateBinaryTask(calcProtocol* serverProtocol,
                         calcProtocol* clientResponse);
void encodeCalcProtocol(calcProtocol* proto);
void decodeCalcMessage(calcMessage* msg);

#endif  // SRC_CLIENT_BINARY_H_
