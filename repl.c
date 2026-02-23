#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#include "repl.h"

const size_t BUF_SIZE = 1024;

void start_repl(void) {

  while (true) {
    char buf[BUF_SIZE];
    printf(">> ");
    fgets(buf, BUF_SIZE, stdin);

    Lexer *lx = Lexer_new(String_from(buf));
    Token t = next_token(lx);
    do {
      print_token(&t);
      free_token(&t);
      t = next_token(lx);

    } while (t.type != TOKEN_ILLEGAL && t.type != TOKEN_EOF);

    free_lexer(lx);
  }
}
