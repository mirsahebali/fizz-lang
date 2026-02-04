#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "lexer.h"

#include "parser.h"

#include "repl.h"

#define CSTRING_IMPLEMENTATION
#include <cstring.h/cstring.h>

void test_token_scanning(void);
void test_start_repl_stdin(void);
void test_let_statements(void);
void test_return_statments(void);
void test_string_parser(void);

int main() {

  test_token_scanning();
  // test_start_repl_stdin();
  test_let_statements();
  test_return_statments();
  return 0;
}

void test_string_parser(void) {
  StatementsArray statements = statements_array_init(2);
  Token token = (Token){LET, String_from("let")};
  Identifier *ident =
      ident_new((Token){IDENT, String_from("myVar")}, String_from("myVar"));
  Expression *value = (Expression *)ident_new(
      (Token){IDENT, String_from("anotherVar")}, String_from("anotherVar"));
  LetStatement *lt_st = let_statement_new(token, ident, value);
  statements_push(&statements, (Node *)&lt_st);
  Program prog = (Program){statements};
}

void test_return_statments(void) {
  const char *input = "return 1;"
                      "return ab;"
                      "return bc;";

  Lexer *l = Lexer_new(String_from(input));

  assert(l != NULL);

  Parser *p = Parser_new(l);
  Program *program = parse_program(p);

  if (p->errors.size != 0) {
    print_errors(p);
    assert(false);
  }

  assert(program->statements.size == 3);

  free_parser(p);
  free_program(program);
}

void test_let_statements(void) {
  const char *input = "let x = 1;"
                      "let foo = 20;"
                      "let hello = 88833;";

  Lexer *l = Lexer_new(String_from(input));
  assert(l != NULL);
  Parser *p = Parser_new(l);
  assert(p != NULL);

  Program *program = parse_program(p);

  if (p->errors.size != 0) {
    print_errors(p);
    assert(false);
  }
  assert(statements_size(&program->statements) == 3);

  assert(program != NULL);

  free_parser(p);
  free_program(program);
}

void test_token_scanning(void) {
  const char *input = "let a = 10; "
                      "let b = 5; "
                      "let add = fn(a, b){ "
                      "return a + b; "
                      "}; "
                      "let result = add(a, b); ";

  Lexer *l = Lexer_new(String_from(input));

  Token expected[] = {
      {LET, String_from("let")},       {IDENT, String_from("a")},
      {ASSIGN, String_from("=")},      {INT, String_from("10")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("b")},
      {ASSIGN, String_from("=")},      {INT, String_from("5")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("add")},
      {ASSIGN, String_from("=")},      {FUNCTION, String_from("fn")},
      {LPAREN, String_from("(")},      {IDENT, String_from("a")},
      {COMMA, String_from(",")},       {IDENT, String_from("b")},
      {RPAREN, String_from(")")},      {LBRACE, String_from("{")},
      {RETURN, String_from("return")}, {IDENT, String_from("a")},
      {PLUS, String_from("+")},        {IDENT, String_from("b")},
      {SEMICOLON, String_from(";")},   {RBRACE, String_from("}")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("result")},
      {ASSIGN, String_from("=")},      {IDENT, String_from("add")},
      {LPAREN, String_from("(")},      {IDENT, String_from("a")},
      {COMMA, String_from(",")},       {IDENT, String_from("b")},
      {RPAREN, String_from(")")},      {SEMICOLON, String_from(";")},
  };

  Token t = next_token(l);
  free_token(&t);

  for (size_t i = 0; i < (sizeof(expected) / sizeof(Token)); i++) {
    t = next_token(l);
    assert(expected[i].type == t.type);
    assert(String_cmp(&expected[i].literal, &t.literal));
    free_string(&t.literal);
    free_string(&expected[i].literal);
  }

  for (size_t i = 0; i < sizeof(expected) / sizeof(Token); i++) {
    free_token(&expected[i]);
  }

  free_lexer(l);

  const char *input1 = "let five = 5;"
                       "let ten = 10;"
                       "let add = fn(x, y) {"
                       "return x + y;"
                       "};"
                       "let result = add(five, ten);"
                       "!-/*5;"
                       "5 < 10 > 5;"
                       "if (5 < 10) {"
                       "return true;"
                       "} else {"
                       "return false;"
                       "}"
                       "10 == 10;"
                       "10 != 9;";

  l = Lexer_new(String_from(input1));

  Token expected1[] = {
      {LET, String_from("let")},       {IDENT, String_from("five")},
      {ASSIGN, String_from("=")},      {INT, String_from("5")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("ten")},
      {ASSIGN, String_from("=")},      {INT, String_from("10")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("add")},
      {ASSIGN, String_from("=")},      {FUNCTION, String_from("fn")},
      {LPAREN, String_from("(")},      {IDENT, String_from("x")},
      {COMMA, String_from(",")},       {IDENT, String_from("y")},
      {RPAREN, String_from(")")},      {LBRACE, String_from("{")},
      {RETURN, String_from("return")}, {IDENT, String_from("x")},
      {PLUS, String_from("+")},        {IDENT, String_from("y")},
      {SEMICOLON, String_from(";")},   {RBRACE, String_from("}")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},       {IDENT, String_from("result")},
      {ASSIGN, String_from("=")},      {IDENT, String_from("add")},
      {LPAREN, String_from("(")},      {IDENT, String_from("five")},
      {COMMA, String_from(",")},       {IDENT, String_from("ten")},
      {RPAREN, String_from(")")},      {SEMICOLON, String_from(";")},

      {BANG, String_from("!")},        {MINUS, String_from("-")},
      {SLASH, String_from("/")},       {ASTERISK, String_from("*")},
      {INT, String_from("5")},         {SEMICOLON, String_from(";")},

      {INT, String_from("5")},         {LT, String_from("<")},
      {INT, String_from("10")},        {GT, String_from(">")},
      {INT, String_from("5")},         {SEMICOLON, String_from(";")},

      {IF, String_from("if")},         {LPAREN, String_from("(")},
      {INT, String_from("5")},         {LT, String_from("<")},
      {INT, String_from("10")},        {RPAREN, String_from(")")},
      {LBRACE, String_from("{")},      {RETURN, String_from("return")},
      {TRUE, String_from("true")},     {SEMICOLON, String_from(";")},
      {RBRACE, String_from("}")},      {ELSE, String_from("else")},
      {LBRACE, String_from("{")},      {RETURN, String_from("return")},
      {FALSE, String_from("false")},   {SEMICOLON, String_from(";")},
      {RBRACE, String_from("}")},

      {INT, String_from("10")},        {EQ, String_from("==")},
      {INT, String_from("10")},        {SEMICOLON, String_from(";")},

      {INT, String_from("10")},        {NOT_EQ, String_from("!=")},
      {INT, String_from("9")},         {SEMICOLON, String_from(";")},
  };

  Token t1 = next_token(l);
  free_token(&t1);
  for (size_t i = 0; i < (sizeof(expected1) / sizeof(Token)); i++) {

    t1 = next_token(l);
    print_token(&t1);

    assert(expected1[i].type == t1.type);
    assert(String_cmp(&expected1[i].literal, &t1.literal));
    free_string(&t1.literal);
    free_token(&t1);
  }

  for (size_t i = 0; i < sizeof(expected1) / sizeof(Token); i++) {
    free_string(&expected1[i].literal);
    free_token(&expected1[i]);
  }
  free_lexer(l);

  printf("Lexical Scanning Passed\n");
}

void test_start_repl_stdin(void) { start_repl(); }
