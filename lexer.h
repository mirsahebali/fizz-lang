#ifndef LEXER_H
#define LEXER_H

#include "utils.h"

typedef enum u8 {
  ILLEGAL,
  IDENT,
  INT,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,

  // Operators
  ASSIGN,   // =
  PLUS,     // +
  MINUS,    // -
  EQ,       // ==
  NOT_EQ,   // !=
  BANG,     // !
  SLASH,    // /
  ASTERISK, // *
  LT,       // <
  GT,       // >

  // Keywords
  FUNCTION, // fn
  LET,      // let
  IF,
  ELSE,
  RETURN,
  TRUE,
  FALSE,
  FOR,

  EOF_T,
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
Token Token_new(TokenType type, String literal);
Token Token_from_char(TokenType type, char ch);
TokenType lookup_ident(String *);

Token next_token(Lexer *);

void print_token(Token *);

void free_token(Token *);
void free_lexer(Lexer *);
#endif // !LEXER_H
