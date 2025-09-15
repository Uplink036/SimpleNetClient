#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define DEBUG 1
#define DEBUG_FUNCTION(msg, ...) if (DEBUG) printf(msg, __VA_ARGS__)


#endif