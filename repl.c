#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

#include "repl.h"

const size_t BUF_SIZE = 1024;

void start_repl(void) {

  char buf[BUF_SIZE];

  Lexer *lx = Lexer_new(String_from(""));
  while (true) {
    printf(">> ");
    char *out = fgets(buf, BUF_SIZE, stdin);

    lx->input = String_from(buf);
    Token t = next_token(lx);
    do {
      print_token(&t);
      free_token(&t);
      t = next_token(lx);

    } while (t.type != ILLEGAL);
  }

  free_lexer(lx);
}
