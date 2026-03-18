#ifndef SRC_CLIENT_TEXT_H_
#define SRC_CLIENT_TEXT_H_

#include <cmath>
#include <cstring>
#include "src/lib/calc.h"
#include "src/lib/macros.h"
#include "src/debug.h"
#include <cstddef>

enum op stringToOp(char* input);
bool parseTextTask(char* msg, char* operation, int* valueOne, int* valueTwo);
int calculateTextTask(char* msg);
void formatTextResult(int result, char* buf, size_t bufSize);

#endif  // SRC_CLIENT_TEXT_H_
