#include "src/client/text.h"

enum op stringToOp(char* input) {
  DEBUG_FUNCTION("client::text::stringToOp(%s)", input);
  IF_ZERO(strcmp("add", input))
    return op::ADD;
  IF_ZERO(strcmp("sub", input))
    return op::SUB;
  IF_ZERO(strcmp("mul", input))
    return op::MUL;
  IF_ZERO(strcmp("div", input))
    return op::DIV;
  printf("ERROR: GOT UNEXPECTED OP COMMAND\n");
  DEBUG_FUNCTION("client::text::stringToOp - %s is not a defined op", input);
  exit(EXIT_FAILURE);
}


bool parseTextTask(char* msg, char* operation, int* valueOne, int* valueTwo) {
  DEBUG_FUNCTION("client::text::parseTextTask(%s)\n", msg);
  IF_NEGATIVE(sscanf(msg, "%s %d %d", operation, valueOne, valueTwo)) {
    printf("ERROR\n");
    DEBUG_FUNCTION("client::text::parseTextTask - Could not parse: %s", msg);
    return false;
  }
  printf("ASSIGNMENT: %s %d %d\n", operation, *valueOne, *valueTwo);
  return true;
}

int calculateTextTask(char* msg) {
  DEBUG_FUNCTION("client::text::calculateTextTask(%s)\n", msg);
  char operation[5];
  int valueOne, valueTwo;
  if (NOT parseTextTask(msg, operation, &valueOne, &valueTwo))
    exit(EXIT_FAILURE);
  int result;
  double temp;
  switch (stringToOp(operation)) {
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
  DEBUG_FUNCTION("client::text::calculateTextTask - Calculated %d\n", result);
  return result;
}

void formatTextResult(int result, char* buf, size_t bufSize) {
  DEBUG_FUNCTION("client::text::formatTextResult(%d)\n", result);
  snprintf(buf, bufSize, "%d\n", result);
}

