#ifndef SRC_CLIENT_CALCULATETASK_H_
#define SRC_CLIENT_CALCULATETASK_H_

#include <cmath>
#include <cstring>
#include "src/lib/calc.h"
#include "src/lib/macros.h"
#include "src/debug.h"

enum op stringToOp(char* input);
int calculateServerTask(char* msg);

#endif  // SRC_CLIENT_CALCULATETASK_H_
