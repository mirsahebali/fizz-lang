#ifndef LEXER_H
#define LEXER_H

#include "utils.h"

typedef enum u8 {
  ILLEGAL,
  IDENT,
  INT,
  ASSIGN,
  PLUS,
  MINUS,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  FUNCTION,
  LET,
  EOF_T,
} TokenType;

String enum_to_string(TokenType);

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

Lexer *Lexer_new(String input);
Token Token_new(TokenType type, String literal);
Token Token_from_char(TokenType type, char ch);
TokenType lookup_ident(String *);

Token next_token(Lexer *l);

void free_token(Token *);
void free_lexer(Lexer *);
#endif // !LEXER_H
