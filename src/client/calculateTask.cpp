#include "calculateTask.h"

enum op stringToOp(char* input)
{
  DEBUG_FUNCTION("client::main::stringToOp(%s)", input);
  IF_ZERO(strcmp("add", input))
    return op::ADD;
  IF_ZERO(strcmp("sub", input))
    return op::SUB;
  IF_ZERO(strcmp("mul", input))
    return op::MUL;
  IF_ZERO(strcmp("div", input))
    return op::DIV;
  printf("ERROR: GOT UNEXPECTED OP COMMAND\n");
  DEBUG_FUNCTION("ERROR - %s is not a defined op", input);
  exit(EXIT_FAILURE);
}


int calculateServerTask(char* msg)
{
  DEBUG_FUNCTION("client::main::calculateServerTask(%s)\n", msg);
  char operation[10];
  int valueOne, valueTwo;
  IF_NEGATIVE(sscanf(msg, "%s %d %d", operation, &valueOne, &valueTwo))
  {
    printf("ERROR\n");
    DEBUG_FUNCTION("Could not properly read scaned values - %s", msg);
    exit(EXIT_FAILURE);
  }    
  printf("ASSIGNMENT: %s %d %d\n", operation, valueOne, valueTwo);
  int result;
  double temp;
  switch (stringToOp(operation))
  {
  case op::ADD:
      result = valueOne + valueTwo;
      break;
  case op::SUB:
      result = valueOne - valueTwo;
      break;
  case op::MUL:
      result = valueOne * valueTwo;
      break;
  case op::DIV:
      temp = valueOne / valueTwo;
      result = round(temp);
      break;
  default:
    break;
  }
  DEBUG_FUNCTION("Calculated %d\n", result);
  return result;
}

