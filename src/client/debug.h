#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define DEBUG_CLIENT 1
#define DEBUG_FUNCTION(msg, ...) if (DEBUG_CLIENT) printf(msg, __VA_ARGS__);


#endif