#ifndef SRC_CLIENT_TEXT_H_
#define SRC_CLIENT_TEXT_H_

#include <cmath>
#include <cstring>
#include "src/lib/calc.h"
#include "src/lib/macros.h"
#include "src/debug.h"

enum op stringToOp(char* input);
int calculateTextTask(char* msg);

#endif  // SRC_CLIENT_TEXT_H_
