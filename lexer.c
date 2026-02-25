#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

void read_char(Lexer *self) {
  if (self->read_position >= self->input.length) {
    self->ch = '\0';
  } else {
    self->ch = String_char_at(&self->input, self->read_position);
  }
  self->position = self->read_position;
  self->read_position++;
}

String read_ident(Lexer *self) {
  int32_t position = self->position;
  while (is_letter(self->ch)) {
    read_char(self);
  }

  return String_substr_range(&self->input, position, self->position - position);
}

char peek_char(const Lexer *self) {
  if (self->read_position >= self->input.length) {
    return 0;
  }

  return String_char_at(&self->input, self->read_position);
}

String read_number(Lexer *self) {
  int32_t position = self->position;
  while (is_digit(self->ch)) {
    read_char(self);
  }

  return String_substr_range(&self->input, position, self->position - position);
}

void skip_whitespace(Lexer *self) {
  while (self->ch == ' ' || self->ch == '\t' || self->ch == '\n' ||
         self->ch == '\r') {
    read_char(self);
  }
}

Lexer *Lexer_new(String input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  assert(lexer != NULL);

  lexer->input = input;
  lexer->position = 0;
  lexer->read_position = 0;
  lexer->ch = 0;

  return lexer;
}

Token Token_from_char(TokenType type, char ch) {
  return (Token){type, String_from_char(ch)};
}

Token Token_clone(const Token *src) {
  return (Token){src->type, String_clone(&src->literal)};
}
TokenType lookup_ident(String *literal) {
  for (size_t i = 0; i < sizeof(keywords_map) / sizeof(KeywordsMap); i++) {
    if (strcmp(literal->chars, keywords_map[i].literal) == 0) {
      return keywords_map[i].type;
    }
  }
  return TOKEN_IDENT;
}

Token next_token(Lexer *l) {
  Token t;

  skip_whitespace(l);
  switch (l->ch) {
  case '=':
    if (peek_char(l) == '=') {
      read_char(l);
      t.type = TOKEN_EQ;
      t.literal = String_from("==");
    } else
      t = Token_from_char(TOKEN_ASSIGN, l->ch);
    break;
  case '+':
    t = Token_from_char(TOKEN_PLUS, l->ch);
    break;
  case '-':
    t = Token_from_char(TOKEN_MINUS, l->ch);
    break;
  case ',':
    t = Token_from_char(TOKEN_COMMA, l->ch);
    break;
  case '/':
    t = Token_from_char(TOKEN_SLASH, l->ch);
    break;
  case '*':
    t = Token_from_char(TOKEN_ASTERISK, l->ch);
    break;
  case '<':
    t = Token_from_char(TOKEN_LT, l->ch);
    break;
  case '>':
    t = Token_from_char(TOKEN_GT, l->ch);
    break;
  case '!':
    if (peek_char(l) == '=') {
      read_char(l);
      t = (Token){TOKEN_NOT_EQ, String_from("!=")};
    } else {
      t = Token_from_char(TOKEN_BANG, l->ch);
    }
    break;
  case '(':
    t = Token_from_char(TOKEN_LPAREN, l->ch);
    break;
  case ')':
    t = Token_from_char(TOKEN_RPAREN, l->ch);
    break;
  case '{':
    t = Token_from_char(TOKEN_LBRACE, l->ch);
    break;
  case '}':
    t = Token_from_char(TOKEN_RBRACE, l->ch);
    break;
  case ';':
    t = Token_from_char(TOKEN_SEMICOLON, l->ch);
    break;
  case '\0':
    t = Token_from_char(TOKEN_EOF, '\0');
    break;
  default:
    if (is_letter(l->ch)) {
      t.literal = read_ident(l);
      t.type = lookup_ident(&t.literal);
      return t;
    } else if (is_digit(l->ch)) {
      t.literal = read_number(l);
      t.type = TOKEN_INT;
      return t;
    } else {
      t = Token_from_char(TOKEN_ILLEGAL, l->ch);
    }
    break;
  }

  read_char(l);

#ifdef DEBUG_PRINTS
  print_token(&t);
#endif /* ifdef DEBUG_PRINTS */

  return t;
}

void print_token(Token *t) {
  printf("t.type = %s\n", token_type_to_string(t->type));
  printf("t.literal = %s\n", t->literal.chars);
}

void free_token(Token *t) {
  if (t == NULL || t->literal.chars == NULL)
    return;

  t->type = 0;
  free_string(&t->literal);
}

void free_lexer(Lexer *l) {
  l->ch = '\0';
  l->read_position = 0;
  l->position = 0;

  free_string(&l->input);
  free(l);
}

const char *token_type_to_string(TokenType tt) {
  static const char *token_names[] = {
#define X(token) #token,
      TOKEN_LIST
#undef X
  };

  assert(tt >= 0 && tt <= TOKEN_COUNT);

  return token_names[tt];
}
