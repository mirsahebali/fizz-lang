#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "utils.h"

#include "lexer.h"

#include "parser.h"

#include "repl.h"

#define CSTRING_IMPLEMENTATION
#include <cstring.h/cstring.h>

#define TEST_STARTED printf("`%s` started\n", __FUNCTION__)
#define TEST_PASSED printf("`%s` passed\n", __FUNCTION__)
#define TEST_FAILED printf("`%s` failed\n", __FUNCTION__)

#define FN_TEST(test_name, expr)                                               \
  void test_name(void) {                                                       \
    TEST_STARTED;                                                              \
    do {                                                                       \
      expr;                                                                    \
    } while (0);                                                               \
    TEST_PASSED;                                                               \
  }

void check_parser_errors(Parser *p);

void test_token_scanning(void);
void test_start_repl_stdin(void);
void test_let_statements(void);
void test_return_statments(void);
void test_string_parser(void);
void test_parsing_ident_expr(void);
void test_parsing_int_expr(void);
void test_parsing_prefix_expr(void);

int main() {

  test_token_scanning();
  // test_start_repl_stdin();
  test_let_statements();
  test_return_statments();
  test_string_parser();
  test_parsing_ident_expr();
  test_parsing_int_expr();

  return 0;
}
typedef struct {
  char *input;
  char *op;
  long value;
} PrefixTest;

void test_integer_literal(Expression *expr, int32_t value);

FN_TEST(test_parsing_prefix_expr, ({
          PrefixTest input[] = {
              {"!6;", "!", 6},
              {"-45;", "-", 45},
              {"!9999;", "!", 9999},
          };
          for (int i = 0; i < 3; i++) {
            Lexer *l = Lexer_new(String_from(input[i].input));
            Parser *p = Parser_new(l);
            Program *program = parse_program(p);
            assert(program->statements.size != 1);
            Statement *stmt = statements_get(&program->statements, 0);
            assert(stmt != NULL);
            ExpressionStatement *expr_st = (ExpressionStatement *)stmt;

            PrefixExpression *prefix_expr = (PrefixExpression *)expr_st->expr;
            String op = STR_NEW(input[i].op);
            assert(String_cmp(&prefix_expr->op, &op));
            test_integer_literal(prefix_expr->right, input[i].value);
          }
        }));

FN_TEST(test_parsing_int_expr, ({
          const char *input = "5;";

          Lexer *l = Lexer_new(String_from(input));
          Parser *p = Parser_new(l);
          Program *program = parse_program(p);

          check_parser_errors(p);
          assert(program->statements.size == 1);

          Statement *stmt = statements_get(&program->statements, 0);
          IntExpr *int_expr = (IntExpr *)stmt;
          assert(int_expr->value == 5);
          String integer_token_literal =
              int_expr->base.vt->token_literal((Node *)int_expr);

          String five = STR_NEW("5");
          assert(String_cmp(&integer_token_literal, &five));

          print_errors(p);

          free_string(&integer_token_literal);
          free_parser(p);
          free_program(program);
        }));

void test_parsing_ident_expr(void) {
  TEST_STARTED;
  char *input = "foobar;";
  Lexer *l = Lexer_new(String_from(input));
  Parser *p = Parser_new(l);
  Program *program = parse_program(p);

  check_parser_errors(p);

  Statement *stmt = statements_get(&program->statements, 0);
  assert(stmt != NULL);
  ExpressionStatement *expr_st = (ExpressionStatement *)stmt;
  assert(expr_st != NULL);

  Identifier *ident = (Identifier *)expr_st->expr;
  String foobar_str = String_from("foobar");
  assert(String_cmp(&ident->value, &foobar_str));
  String ident_token_literal_str = ident->base.vt->token_literal((Node *)ident);
  assert(String_cmp(&ident_token_literal_str, &foobar_str));

  free_string(&foobar_str);
  free_string(&ident_token_literal_str);

  free_program(program);
  free_parser(p);
  TEST_PASSED;
}

void test_string_parser(void) {
  TEST_STARTED;
  StatementsArray statements = statements_array_init(2);
  String let_str = STR_NEW("let");
  Token token = (Token){
      LET,
      let_str,
  };
  Identifier *ident =
      ident_new((Token){IDENT, String_from("myVar")}, String_from("myVar"));
  Expression *value = (Expression *)ident_new(
      (Token){IDENT, String_from("anotherVar")}, String_from("anotherVar"));
  LetStatement *lt_st = let_statement_new(token, ident, value);
  statements_push(&statements, (Statement *)lt_st);
  Program prog = (Program){statements};

  String *actual = program_string(&prog);
  String expected = STR_NEW("let myVar = anotherVar;");

  assert(String_cmp(actual, &expected));
  free_string(actual);
  free(actual);
  free_statements(&statements);

  TEST_PASSED;
}

void test_return_statments(void) {
  TEST_STARTED;
  const char *input = "return 1;"
                      "return ab;"
                      "return bc;";

  Lexer *l = Lexer_new(String_from(input));

  assert(l != NULL);

  Parser *p = Parser_new(l);
  Program *program = parse_program(p);
  check_parser_errors(p);

  if (p->errors.size != 0) {
    print_errors(p);
    assert(false);
  }

  assert(program->statements.size == 3);

  for (int i = 0; i < program->statements.size; i++) {
    ReturnStatement *ret_st =
        (ReturnStatement *)statements_get(&program->statements, i);
    assert(ret_st != NULL);
    String ret_token_literal = ret_st->base.vt->token_literal((Node *)ret_st);
    String expected = STR_NEW("return");
    assert(String_cmp(&ret_token_literal, &expected));
    free_string(&ret_token_literal);
  }

  free_parser(p);
  free_program(program);

  TEST_PASSED;
}

void test_let_statements(void) {
  TEST_STARTED;
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
  TEST_PASSED;
}

void test_token_scanning(void) {
  TEST_STARTED;
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

  TEST_PASSED;
}

void test_start_repl_stdin(void) {
  TEST_STARTED;
  start_repl();
  TEST_PASSED;
}

void check_parser_errors(Parser *p) {
  const StringArray *errors = parser_errors(p);
  if (errors->size == 0) {
    return;
  }

  printf("Parser has %d errors", errors->size);
  for (int i = 0; i < errors->size; i++) {
    printf("parser error: %s", string_array_get(&p->errors, i).chars);
  }
  assert(false);
}
