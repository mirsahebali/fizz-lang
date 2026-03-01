#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
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

#define CSTR 0
#define REF_STR 1
#define OWNED_STR 2

#define EVAL_PRINT(specifier, left, right)                                     \
                                                                               \
  printf("Assertion failed at: %s %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
  printf("left: ");                                                            \
  printf("%s = " specifier, #left, left);                                      \
  printf("\n");                                                                \
  printf("right: ");                                                           \
  printf("%s = " specifier, #right, right);                                    \
  printf("\n");

#define ASSERT_EVAL(specifier, left, right)                                    \
  {                                                                            \
    EVAL_PRINT(specifier, left, right);                                        \
    assert(false);                                                             \
  }

#define ASSERT_EQ(specifier, left, right)                                      \
  do {                                                                         \
    if (left != right)                                                         \
      ASSERT_EVAL(specifier, left, right)                                      \
  } while (0)

#define ASSERT_NEQ(specifier, left, right)                                     \
  do {                                                                         \
    if (left == right)                                                         \
      ASSERT_EVAL(specifier, left, right)                                      \
  } while (0)

#define TEST_STARTED printf("`%s` started\n", __FUNCTION__)
#define TEST_PASSED printf("`%s` passed\n", __FUNCTION__)
#define TEST_FAILED printf("`%s` failed\n", __FUNCTION__)

#define FN_TEST(test_name, expr)                                               \
  void test_name(void) {                                                       \
    TEST_STARTED;                                                              \
    do                                                                         \
      expr while (0);                                                          \
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
void test_operator_precedence_parsing(void);
void test_if_expression_parsing(void);
void test_fn_expression_parsing(void);
void test_call_expression_parsing(void);

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
  test_operator_precedence_parsing();
  test_if_expression_parsing();
  test_fn_expression_parsing();
  test_call_expression_parsing();

  return 0;
}

typedef struct {
  const char *input;
  int argument_num;
  char *ident_name;
  StringArray expr_string_list;

} CallExprTestCase;

void test_call_expression_parsing(void) {
  TEST_STARTED;

  CallExprTestCase test_cases[3] = {
      {
          .input = "add(a + b, c / d, x);",
          .argument_num = 3,
          .ident_name = "add",
          .expr_string_list =
              String_array_from_cstr(3, "(a + b)", "(c / d)", "x"),
      },

      {.input = "sub();",
       .argument_num = 0,
       .ident_name = "sub",
       .expr_string_list = string_array_init(0)},

      {
          .input = "mult(a, add(x - y));",
          .argument_num = 2,
          .ident_name = "mult",
          .expr_string_list = String_array_from_cstr(2, "a", "add((x - y))"),
      },
  };

  for (int i = 0; i < 3; i++) {
    CallExprTestCase test_case = test_cases[i];
    Parser *p = Parser_new(Lexer_new(String_from(test_case.input)));
    Program *program = parse_program(p);

    check_parser_errors(p);

    ExpressionStatement *expr_st =
        (ExpressionStatement *)statements_get(&program->statements, 0);

    CallExpression *call_expr = (CallExpression *)expr_st->expr;
    ASSERT_EQ("%d", call_expr->arguments.size, test_case.argument_num);
    Identifier *ident = (Identifier *)call_expr->function;
    String expected = STR_NEW(test_case.ident_name);

    assert(String_cmp(&ident->value, &expected));

    for (int i = 0; i < call_expr->arguments.size; i++) {
      InfixExpression *expr =
          (InfixExpression *)expressions_get(&call_expr->arguments, i);
      String infix_str = expr->base.vt->string((Node *)expr);
      String expected_str = string_array_get(&test_case.expr_string_list, i);

      assert(String_cmp(&infix_str, &expected_str));

      free_string(&infix_str);
      free_string(&expected_str);
    }

    free_parser(p);
    free_program(program);
  }

  for (int i = 0; i < 3; i++) {
    CallExprTestCase test_case = test_cases[i];
    free_string_array(&test_case.expr_string_list);
  }

  TEST_PASSED;
}

typedef struct {
  char *input;
  int parameters;
  StringArray ident_list;
} FnExprTestCase;

void test_fn_expression_parsing(void) {
  TEST_STARTED;
  FnExprTestCase inputs[] = {
      {"fn(x, y){ return x > y;}", 2, String_array_from_cstr(2, "x", "y")},
      {"fn(){ y }", 0, string_array_init(0)},
      {"fn(x, y, z){}", 3, String_array_from_cstr(3, "x", "y", "z")}};

  for (size_t i = 0; i < (sizeof(inputs) / sizeof(inputs[0])); i++) {
    FnExprTestCase input = inputs[i];
    Parser *p = Parser_new(Lexer_new(String_from(input.input)));
    Program *program = parse_program(p);

    printf("input = %s\n", input.input);
    check_parser_errors(p);

    ExpressionStatement *expr_st =
        (ExpressionStatement *)statements_get(&program->statements, 0);

    ASSERT_NEQ("%p", expr_st, NULL);

    FnExpression *fn_expr = (FnExpression *)expr_st->expr;

    ASSERT_NEQ("%p", fn_expr, NULL);

    ASSERT_EQ("%d", fn_expr->parameters.size, input.parameters);

    for (int i = 0; i < input.parameters; i++) {
      String expected_param = string_array_get(&input.ident_list, i);
      String actual_param = (identifiers_get(&fn_expr->parameters, i))->value;
      assert(String_cmp(&expected_param, &actual_param));
      free_string(&expected_param);
    }

    free_program(program);
    free_parser(p);
  }

  for (size_t i = 0; i < (sizeof(inputs) / sizeof(inputs[0])); i++) {
    free_string_array(&inputs[i].ident_list);
  }
  TEST_PASSED;
}

void test_if_expression_parsing(void) {
  TEST_STARTED;
  const char *inputs[] = {"if(x < y){ x }", "if (x < y) { y } else { x }"};

  const char *input = inputs[0];
  Lexer *l = Lexer_new(String_from(input));
  Parser *p = Parser_new(l);

  Program *program = parse_program(p);

  check_parser_errors(p);

  ExpressionStatement *expression_statement =
      (ExpressionStatement *)statements_get(&program->statements, 0);

  ASSERT_NEQ("%p", expression_statement, NULL);

  IfExpression *if_expr = (IfExpression *)expression_statement->expr;

  ASSERT_NEQ("%p", if_expr, NULL);
  ASSERT_NEQ("%p", if_expr->condition, NULL);
  ASSERT_NEQ("%p", if_expr->consequence, NULL);

  InfixExpression *infix_expr = (InfixExpression *)if_expr->condition;
  ASSERT_NEQ("%p", (Identifier *)infix_expr->left, NULL);
  ASSERT_NEQ("%p", (Identifier *)infix_expr->right, NULL);

  ASSERT_EQ("%c", ((Identifier *)infix_expr->left)->value.chars[0], 'x');
  ASSERT_EQ("%c", ((Identifier *)infix_expr->right)->value.chars[0], 'y');

  ExpressionStatement *expr_st = (ExpressionStatement *)statements_get(
      &if_expr->consequence->statements, 0);
  ASSERT_NEQ("%p", expr_st, NULL);
  ASSERT_NEQ("%p", expr_st->expr, NULL);
  ASSERT_EQ("%c", ((Identifier *)expr_st->expr)->value.chars[0], 'x');

  free_program(program);
  free_parser(p);

  input = inputs[1];
  l = Lexer_new(String_from(input));
  p = Parser_new(l);

  program = parse_program(p);

  check_parser_errors(p);

  expression_statement =
      (ExpressionStatement *)statements_get(&program->statements, 0);

  ASSERT_NEQ("%p", expression_statement, NULL);

  if_expr = (IfExpression *)expression_statement->expr;

  ASSERT_NEQ("%p", if_expr, NULL);
  ASSERT_NEQ("%p", if_expr->condition, NULL);
  ASSERT_NEQ("%p", if_expr->consequence, NULL);
  ASSERT_NEQ("%p", if_expr->alternative, NULL);

  infix_expr = (InfixExpression *)if_expr->condition;
  ASSERT_NEQ("%p", (Identifier *)infix_expr->left, NULL);
  ASSERT_NEQ("%p", (Identifier *)infix_expr->right, NULL);

  ASSERT_EQ("%c", ((Identifier *)infix_expr->left)->value.chars[0], 'x');
  ASSERT_EQ("%c", ((Identifier *)infix_expr->right)->value.chars[0], 'y');

  expr_st = (ExpressionStatement *)statements_get(
      &if_expr->consequence->statements, 0);
  ASSERT_NEQ("%p", expr_st, NULL);
  ASSERT_NEQ("%p", expr_st->expr, NULL);
  ASSERT_EQ("%c", ((Identifier *)expr_st->expr)->value.chars[0], 'y');

  ExpressionStatement *expr_st_alt = (ExpressionStatement *)statements_get(
      &if_expr->alternative->statements, 0);
  ASSERT_NEQ("%p", expr_st_alt, NULL);
  ASSERT_NEQ("%p", expr_st_alt->expr, NULL);
  ASSERT_EQ("%c", ((Identifier *)expr_st_alt->expr)->value.chars[0], 'x');

  free_program(program);
  free_parser(p);

  TEST_PASSED;
}

void test_integer_literal(Expression *expr, int32_t value);

typedef struct {
  char *input;
  int left_value;
  char *op;
  int right_value;
} InfixTest;

typedef struct {

  char *input;
  char *expected;
} TestCase;
void test_operator_precedence_parsing(void) {
  TEST_STARTED;
  TestCase test_cases[] = {

      {"(a - b) * c", "((a - b) * c)"},
      {"!(true == true)", "(!(true == true))"},
      {"-(5 + 8)", "(-(5 + 8))"},
      {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
      {"(a + b) / c", "((a + b) / c)"},

      {"true", "true"},
      {"false", "false"},
      {"-a * b", "((-a) * b)"},
      {"-!a", "(-(!a))"},
      {"!-a", "(!(-a))"},
      {"a + b + c", "((a + b) + c)"},
      {"a + b - c", "((a + b) - c)"},
      {"a * b * c", "((a * b) * c)"},
      {"a * b / c", "((a * b) / c)"},
      {"a + b / c", "(a + (b / c))"},
      {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
      {"3 + 4; -5 * 5", "(3 + 4) ((-5) * 5)"},
      {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
      {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},

  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    TestCase test_case = test_cases[i];
    Lexer *l = Lexer_new(String_from(test_case.input));
    Parser *p = Parser_new(l);

    Program *prog = parse_program(p);

    check_parser_errors(p);

    String actual = program_string(prog);
    String expected_out = String_from(test_case.expected);
    if (!String_cmp(&actual, &expected_out)) {
      printf("actual->chars = %s\n", actual.chars);
      printf("expected_out.chars = %s\n", expected_out.chars);
      assert(String_cmp(&actual, &expected_out));
    }

    free_string(&actual);
    free_string(&expected_out);
    free_program(prog);
    free_parser(p);
  }
  TEST_FAILED;
}

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

FN_TEST(test_parsing_int_expr, {
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
});

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

  String actual = program_string(&prog);
  String expected = STR_NEW("let myVar = anotherVar;");

  assert(String_cmp(&actual, &expected));
  free_string(&actual);
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

  check_parser_errors(p);
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
    printf("expected[i].literal = %s\n", expected[i].literal.chars);
    printf("t.literal = %s\n", t.literal.chars);
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

    printf("parser error: %s\n", p->errors.data[i].chars);
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
