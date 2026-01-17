#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "lexer.h"

void test_token_scanning(void);
void test_string_from(void);
void test_string_concat_char(void);
void test_string_concat_str(void);
void test_string_cmp(void);
void test_char_at_str(void);
void test_string_from_char(void);
void test_string_substr(void);

int main() {

  test_string_from();
  test_string_concat_char();
  test_string_concat_str();
  test_char_at_str();
  test_string_from_char();
  test_string_substr();
  test_token_scanning();

  return 0;
}

void test_token_scanning(void) {
  const char *input = "let a = 10;"
                      "let b = 5;"
                      "let add = fn(a, b){"
                      "return a + b; "
                      "};"
                      "let result = add(a, b);";

  Lexer *l = Lexer_new(String_from(input));

  Token expected[] = {
      {LET, String_from("let")},      {IDENT, String_from("a")},
      {ASSIGN, String_from("=")},     {INT, String_from("10")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},      {IDENT, String_from("b")},
      {ASSIGN, String_from("=")},     {INT, String_from("5")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},      {IDENT, String_from("add")},
      {ASSIGN, String_from("=")},     {FUNCTION, String_from("fn")},
      {LPAREN, String_from("(")},     {IDENT, String_from("a")},
      {COMMA, String_from(",")},      {IDENT, String_from("b")},
      {RPAREN, String_from(")")},     {LBRACE, String_from("{")},
      {IDENT, String_from("return")}, {IDENT, String_from("a")},
      {PLUS, String_from("+")},       {IDENT, String_from("b")},
      {SEMICOLON, String_from(";")},  {RBRACE, String_from("}")},
      {SEMICOLON, String_from(";")},

      {LET, String_from("let")},      {IDENT, String_from("result")},
      {ASSIGN, String_from("=")},     {IDENT, String_from("add")},
      {LPAREN, String_from("(")},     {IDENT, String_from("a")},
      {PLUS, String_from(",")},       {IDENT, String_from("b")},
      {RPAREN, String_from(")")},     {SEMICOLON, String_from(";")},
  };

  for (int i = 0; i < (sizeof(expected) / sizeof(Token)); i++) {
    Token t = next_token(l);
    printf("i = %d\n", i);
    printf("expected[i].type = %d\n", expected[i].type);
    printf("expected[i].literal = %s\n", expected[i].literal.chars);
    printf("strlen(expected[i].literal.chars) = %d\n",
           strlen(expected[i].literal.chars));
    printf("t.type = %d\n", t.type);
    printf("t.literal = %s\n", t.literal.chars);
    printf("strlen(t.literal.chars) = %d\n", strlen(t.literal.chars));
    assert(expected[i].type == t.type);
    assert(cmp_str(&expected[i].literal, &t.literal));
    free_string(&t.literal);
    free_string(&expected[i].literal);
  }
  free_lexer(l);
}

void test_string_from(void) {
  String input = String_from("Hello, World");
  String expected = {"Hello, World", 12};

  assert(strcmp(input.chars, expected.chars) == 0);
  assert(input.length == expected.length);
  free_string(&input);
}

void test_string_concat_char(void) {
  String test_str = String_from("12345");

  String input = concat_char(&test_str, '6');
  String expected = String_from("123456");
  assert(strcmp(input.chars, expected.chars) == 0);

  String input1 = concat_char(&input, '7');
  String expected1 = String_from("1234567");
  assert(strcmp(input1.chars, expected1.chars) == 0);

  free_string(&expected1);
  free_string(&input1);
  free_string(&expected);
  free_string(&input);
  free_string(&test_str);
}

void test_string_concat_str(void) {
  String test_str = String_from("Hello");
  String input = concat_str(&test_str, ", World");
  String expected = String_from("Hello, World");

  assert(strcmp(input.chars, expected.chars) == 0);

  free_string(&test_str);
  free_string(&input);
  free_string(&expected);
}

void test_string_cmp(void) {
  String left = String_from("Shawww");
  String right = String_from("Shawww");
  assert(cmp_str(&left, &right));

  free_string(&left);
  free_string(&right);
  left = String_from("Poshanka");
  right = String_from("Shawww");
  assert(!cmp_str(&left, &right));

  free_string(&left);
  free_string(&right);
}

void test_char_at_str(void) {
  String input = String_from("abc123");
  char out = char_at_str(&input, 0);
  char expected = 'a';
  assert(out == expected);

  out = char_at_str(&input, 1);
  expected = 'b';

  assert(out == expected);

  out = char_at_str(&input, 2);
  char invalid = 'd';

  assert(out != invalid);

  out = char_at_str(&input, 5);
  expected = '3';

  assert(out == expected);

  free_string(&input);
}

void test_string_from_char(void) {}

void test_string_substr(void) {
  String input = String_from("Hello, World");
  String substr = substr_range(&input, 2, 5);
  String expected = String_from("llo, ");

  assert(cmp_str(&substr, &expected));
}
