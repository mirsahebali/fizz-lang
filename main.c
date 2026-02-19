#include "parser.h"
#include "repl.h"
#include <stdio.h>

#define CSTRING_IMPLEMENTATION
#include "cstring.h/cstring.h"

int main(void) {

  printf("\n-----------------------------------------\n");
  printf("Hello, Fellow programmer\n");
  printf("Welcome to Fizz Lang\n");
  printf("-----------------------------------------\n");
  start_repl();

  return 0;
}
