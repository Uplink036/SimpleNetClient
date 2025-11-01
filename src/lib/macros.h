
#ifndef SRC_LIB_MACROS_H_
#define SRC_LIB_MACROS_H_

#define AND       &&
#define OR        ||
#define NOT       !
#define NOTEQUALS !=
#define EQUALS    ==
#define IS        =

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define IF_ZERO(stmt) if (stmt EQUALS 0)
#define IF_NOT_ZERO(stmt) if (stmt NOTEQUALS 0)
#define IF_NEGATIVE(stmt) if (stmt < 0)
#define IF_NULL(stmt) if (stmt EQUALS NULL)

#endif  // SRC_LIB_MACROS_H_
