#ifndef TASK_H
#define TASK_H

#include <cmath>
#include <cstring>
#include "calc.h"
#include "macros.h"
#include "debug.h"

enum op stringToOp(char* input);
int calculateServerTask(char* msg);

#endif