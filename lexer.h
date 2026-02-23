#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "cstring.h/cstring.h"

#include "utils.h"

#define TOKEN_LIST                                                             \
  X(ILLEGAL)                                                                   \
  X(IDENT)                                                                     \
  X(INT)                                                                       \
  X(COMMA)                                                                     \
  X(SEMICOLON)                                                                 \
  X(LPAREN)                                                                    \
  X(RPAREN)                                                                    \
  X(LBRACE)                                                                    \
  X(RBRACE)                                                                    \
  X(ASSIGN)                                                                    \
  X(PLUS)                                                                      \
  X(MINUS)                                                                     \
  X(EQ)                                                                        \
  X(NOT_EQ)                                                                    \
  X(BANG)                                                                      \
  X(SLASH)                                                                     \
  X(ASTERISK)                                                                  \
  X(LT)                                                                        \
  X(GT)                                                                        \
  X(FUNCTION)                                                                  \
  X(LET)                                                                       \
  X(IF)                                                                        \
  X(ELSE)                                                                      \
  X(RETURN)                                                                    \
  X(TRUE)                                                                      \
  X(FALSE)                                                                     \
  X(FOR)                                                                       \
  X(EOF_T)

typedef enum u8 {
#define X(tt) tt,
  TOKEN_LIST
#undef X
      TOKEN_COUNT
} TokenType;

const char *token_type_to_string(TokenType);

typedef struct {
  TokenType type;
  String literal;
} Token;

typedef struct {
  String input;
  int position;
  int read_position;
  char ch;
} Lexer;

typedef struct {
  const char *literal;
  TokenType type;
} KeywordsMap;

static const KeywordsMap keywords_map[] = {
    {"let", LET},       {"fn", FUNCTION}, {"if", IF},       {"else", ELSE},
    {"return", RETURN}, {"true", TRUE},   {"false", FALSE}, {"for", FOR},
};

Lexer *Lexer_new(String input);
Token Token_clone(const Token *);
Token Token_from_char(TokenType type, char ch);
TokenType lookup_ident(String *);

// INFO: start the lexing by doing next_token and free_token first, so we can
// get rid of the initial illegal character or we can also remove it by popping
// the first element
Token next_token(Lexer *);

void print_token(Token *);

void free_token(Token *);
void free_lexer(Lexer *);
#endif // !LEXER_H
