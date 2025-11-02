#ifndef SRC_DEBUG_H_
#define SRC_DEBUG_H_

#include <stdio.h>

#define DEBUG 0
#define DEBUG_FUNCTION(...) if (DEBUG) printf(__VA_ARGS__)


#endif  // SRC_DEBUG_H_
