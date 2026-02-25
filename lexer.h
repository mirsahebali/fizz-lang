#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "cstring.h/cstring.h"

#include "utils.h"

#define TOKEN_LIST                                                             \
  X(TOKEN_ILLEGAL)                                                             \
  X(TOKEN_IDENT)                                                               \
  X(TOKEN_INT)                                                                 \
  X(TOKEN_COMMA)                                                               \
  X(TOKEN_SEMICOLON)                                                           \
  X(TOKEN_LPAREN)                                                              \
  X(TOKEN_RPAREN)                                                              \
  X(TOKEN_LBRACE)                                                              \
  X(TOKEN_RBRACE)                                                              \
  X(TOKEN_ASSIGN)                                                              \
  X(TOKEN_PLUS)                                                                \
  X(TOKEN_MINUS)                                                               \
  X(TOKEN_EQ)                                                                  \
  X(TOKEN_NOT_EQ)                                                              \
  X(TOKEN_BANG)                                                                \
  X(TOKEN_SLASH)                                                               \
  X(TOKEN_ASTERISK)                                                            \
  X(TOKEN_LT)                                                                  \
  X(TOKEN_GT)                                                                  \
  X(TOKEN_FUNCTION)                                                            \
  X(TOKEN_LET)                                                                 \
  X(TOKEN_IF)                                                                  \
  X(TOKEN_ELSE)                                                                \
  X(TOKEN_RETURN)                                                              \
  X(TOKEN_TRUE)                                                                \
  X(TOKEN_FALSE)                                                               \
  X(TOKEN_FOR)                                                                 \
  X(TOKEN_EOF)

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
    {"let", TOKEN_LET},       {"fn", TOKEN_FUNCTION},
    {"if", TOKEN_IF},         {"else", TOKEN_ELSE},

    {"return", TOKEN_RETURN}, {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},   {"for", TOKEN_FOR},
};

Lexer *Lexer_new(String input);
Token Token_clone(const Token *);
Token Token_from_char(TokenType type, char ch);
TokenType lookup_ident(String *);

// INFO: start the lexing by doing next_token and free_token first, so we can
// get rid of the initial illegal character or we can also remove it by popping
// the first element
Token next_token(Lexer *self);

void print_token(Token *);

void free_token(Token *self);
void free_lexer(Lexer *self);
#endif // !LEXER_H
