
#include "lexer.h"

#include "parser.h"

#define CSTRING_IMPLEMENTATION
#include "cstring.h/cstring.h"

const size_t BUF_SIZE = 1024;

int main(void) {

  while (true) {
    char buf[BUF_SIZE];
    printf(">> ");
    fgets(buf, BUF_SIZE, stdin);

    Parser *p = Parser_new(Lexer_new(String_from(buf)));
    Program *prog = parse_program(p);

    String out = program_string(prog);

    printf("%s\n", out.chars);

    if (p->errors.size != 0) {
      print_errors(p);
      exit(0);
    }

    free_string(&out);
    free_program(prog);
    free_parser(p);
  }
  return EXIT_SUCCESS;
}
