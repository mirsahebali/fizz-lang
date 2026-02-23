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
void test_parsing_infix_expr(void);

int main() {

  test_token_scanning();
  // test_start_repl_stdin();
  test_let_statements();
  test_return_statments();
  test_string_parser();
  test_parsing_ident_expr();
  test_parsing_int_expr();
  test_parsing_prefix_expr();
  test_parsing_infix_expr();

  return 0;
}

void test_integer_literal(Expression *expr, int32_t value);

typedef struct {
  char *input;
  int left_value;
  char *op;
  int right_value;
} InfixTest;

void test_parsing_infix_expr(void) {
  TEST_STARTED;
  InfixTest input[] = {{"5 + 5;", 5, "+", 5},   {"6 * 6;", 6, "*", 6},
                       {"7 - 5;", 7, "-", 5},   {"9 / 3;", 9, "/", 3},
                       {"3 == 2;", 3, "==", 2}, {"1 != 0;", 1, "!=", 0},
                       {"8 < 1;", 8, "<", 1}};
  for (size_t i = 0; i < (sizeof(input) / sizeof(input[0])); i++) {
    InfixTest expected = input[i];
    Lexer *l = Lexer_new(String_from(expected.input));
    Parser *p = Parser_new(l);

    Program *program = parse_program(p);

    check_parser_errors(p);

    assert(program->statements.size != 0);
    assert(program->statements.size == 1);

    Statement *stmt = statements_get(&program->statements, 0);
    assert(stmt != NULL);

    ExpressionStatement *expr_st = (ExpressionStatement *)stmt;
    InfixExpression *infix_expr = (InfixExpression *)expr_st->expr;

    String expected_op = STR_NEW(expected.op);
    assert(String_cmp(&infix_expr->op, &expected_op));

    test_integer_literal(infix_expr->right, expected.right_value);
    test_integer_literal(infix_expr->left, expected.left_value);

    free_parser(p);
    free_program(program);
  }
  TEST_PASSED;
}

typedef struct {
  char *input;
  char *op;
  long value;
} PrefixTest;

void test_parsing_prefix_expr(void) {
  TEST_STARTED;
  PrefixTest input[] = {
      {"!6;", "!", 6},
      {"-45;", "-", 45},
      {"!9999;", "!", 9999},
  };
  for (int i = 0; i < 3; i++) {
    Lexer *l = Lexer_new(String_from(input[i].input));
    Parser *p = Parser_new(l);
    Program *program = parse_program(p);
    check_parser_errors(p);
    assert(program->statements.size == 1);
    Statement *stmt = statements_get(&program->statements, 0);

    assert(stmt != NULL);
    ExpressionStatement *expr_st = (ExpressionStatement *)stmt;

    PrefixExpression *prefix_expr = (PrefixExpression *)expr_st->expr;
    String op = STR_NEW(input[i].op);
    assert(String_cmp(&prefix_expr->op, &op));
    test_integer_literal(prefix_expr->right, input[i].value);

    free_program(program);
    free_parser(p);
  }
  TEST_PASSED;
}

FN_TEST(test_parsing_int_expr, ({
          const char *input = "5;";

          Lexer *l = Lexer_new(String_from(input));
          Parser *p = Parser_new(l);
          Program *program = parse_program(p);

          check_parser_errors(p);
          assert(program->statements.size == 1);

          ExpressionStatement *expr_stmt =
              (ExpressionStatement *)statements_get(&program->statements, 0);
          IntExpr *int_expr = (IntExpr *)expr_stmt->expr;
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
      TOKEN_LET,
      let_str,
  };
  Identifier *ident = ident_new((Token){TOKEN_IDENT, String_from("myVar")},
                                String_from("myVar"));
  Expression *value =
      (Expression *)ident_new((Token){TOKEN_IDENT, String_from("anotherVar")},
                              String_from("anotherVar"));
  LetStatement *lt_st = let_statement_new(Token_clone(&token), ident, value);
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
      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("a")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_INT, String_from("10")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("b")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_INT, String_from("5")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("add")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_FUNCTION, String_from("fn")},
      {TOKEN_LPAREN, String_from("(")},
      {TOKEN_IDENT, String_from("a")},
      {TOKEN_COMMA, String_from(",")},
      {TOKEN_IDENT, String_from("b")},
      {TOKEN_RPAREN, String_from(")")},
      {TOKEN_LBRACE, String_from("{")},
      {TOKEN_RETURN, String_from("return")},
      {TOKEN_IDENT, String_from("a")},
      {TOKEN_PLUS, String_from("+")},
      {TOKEN_IDENT, String_from("b")},
      {TOKEN_SEMICOLON, String_from(";")},
      {TOKEN_RBRACE, String_from("}")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("result")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_IDENT, String_from("add")},
      {TOKEN_LPAREN, String_from("(")},
      {TOKEN_IDENT, String_from("a")},
      {TOKEN_COMMA, String_from(",")},
      {TOKEN_IDENT, String_from("b")},
      {TOKEN_RPAREN, String_from(")")},
      {TOKEN_SEMICOLON, String_from(";")},
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
      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("five")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_INT, String_from("5")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("ten")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_INT, String_from("10")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("add")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_FUNCTION, String_from("fn")},
      {TOKEN_LPAREN, String_from("(")},
      {TOKEN_IDENT, String_from("x")},
      {TOKEN_COMMA, String_from(",")},
      {TOKEN_IDENT, String_from("y")},
      {TOKEN_RPAREN, String_from(")")},
      {TOKEN_LBRACE, String_from("{")},
      {TOKEN_RETURN, String_from("return")},
      {TOKEN_IDENT, String_from("x")},
      {TOKEN_PLUS, String_from("+")},
      {TOKEN_IDENT, String_from("y")},
      {TOKEN_SEMICOLON, String_from(";")},
      {TOKEN_RBRACE, String_from("}")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_LET, String_from("let")},
      {TOKEN_IDENT, String_from("result")},
      {TOKEN_ASSIGN, String_from("=")},
      {TOKEN_IDENT, String_from("add")},
      {TOKEN_LPAREN, String_from("(")},
      {TOKEN_IDENT, String_from("five")},
      {TOKEN_COMMA, String_from(",")},
      {TOKEN_IDENT, String_from("ten")},
      {TOKEN_RPAREN, String_from(")")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_BANG, String_from("!")},
      {TOKEN_MINUS, String_from("-")},
      {TOKEN_SLASH, String_from("/")},
      {TOKEN_ASTERISK, String_from("*")},
      {TOKEN_INT, String_from("5")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_INT, String_from("5")},
      {TOKEN_LT, String_from("<")},
      {TOKEN_INT, String_from("10")},
      {TOKEN_GT, String_from(">")},
      {TOKEN_INT, String_from("5")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_IF, String_from("if")},
      {TOKEN_LPAREN, String_from("(")},
      {TOKEN_INT, String_from("5")},
      {TOKEN_LT, String_from("<")},
      {TOKEN_INT, String_from("10")},
      {TOKEN_RPAREN, String_from(")")},
      {TOKEN_LBRACE, String_from("{")},
      {TOKEN_RETURN, String_from("return")},
      {TOKEN_TRUE, String_from("true")},
      {TOKEN_SEMICOLON, String_from(";")},
      {TOKEN_RBRACE, String_from("}")},
      {TOKEN_ELSE, String_from("else")},
      {TOKEN_LBRACE, String_from("{")},
      {TOKEN_RETURN, String_from("return")},
      {TOKEN_FALSE, String_from("false")},
      {TOKEN_SEMICOLON, String_from(";")},
      {TOKEN_RBRACE, String_from("}")},

      {TOKEN_INT, String_from("10")},
      {TOKEN_EQ, String_from("==")},
      {TOKEN_INT, String_from("10")},
      {TOKEN_SEMICOLON, String_from(";")},

      {TOKEN_INT, String_from("10")},
      {TOKEN_NOT_EQ, String_from("!=")},
      {TOKEN_INT, String_from("9")},
      {TOKEN_SEMICOLON, String_from(";")},
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

  printf("Parser has %d errors\n", errors->size);
  for (int i = 0; i < errors->size; i++) {
    printf("parser error: %s\n", string_array_get(&p->errors, i).chars);
  }
  print_errors(p);
  assert(false);
}
void test_integer_literal(Expression *expr, int32_t value) {
  IntExpr *int_expr = (IntExpr *)expr;
  assert(int_expr->value == value);

  String int_expr_str = int_expr->base.vt->string((Node *)int_expr);

  String expected = String_from_int(value);
  assert(String_cmp(&int_expr_str, &expected));

  free_string(&int_expr_str);
  free_string(&expected);
}
