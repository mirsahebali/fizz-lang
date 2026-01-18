#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

const char *token_type_to_string(TokenType tt) {
  switch (tt) {

  case ILLEGAL:
    return "ILLEGAL";
  case IDENT:
    return "IDENT";
  case INT:
    return "INT";
  case ASSIGN:
    return "ASSIGN";
  case PLUS:
    return "PLUS";
  case MINUS:
    return "MINUS";
  case COMMA:
    return "COMMA";
  case SEMICOLON:
    return "SEMICOLON";
  case LPAREN:
    return "LPAREN";
  case RPAREN:
    return "RPAREN";
  case LBRACE:
    return "LBRACE";
  case RBRACE:
    return "RBRACE";
  case FUNCTION:
    return "FUNCTION";
  case LET:
    return "LET";
  case IF:
    return "IF";
  case ELSE:
    return "ELSE";
  case RETURN:
    return "RETURN";
  case TRUE:
    return "TRUE";
  case FALSE:
    return "FALSE";
  case FOR:
    return "FOR";
  case EOF_T:
    return "EOF_T";

  case BANG:
    return "BANG";

  default:
    return "ILLEGAL";
  }
}

void read_char(Lexer *l) {
  if (l->read_position >= l->input.length) {
    l->ch = '\0';
  } else {
    l->ch = char_at_str(&l->input, l->read_position);
  }
  l->position = l->read_position;
  l->read_position++;
}

String read_ident(Lexer *l) {
  int32_t position = l->position;
  while (is_letter(l->ch)) {
    read_char(l);
  }

  return substr_range(&l->input, position, l->position - position);
}

char peek_char(Lexer *l) {
  if (l->read_position >= l->input.length) {
    return 0;
  }

  return char_at_str(&l->input, l->read_position);
}

String read_number(Lexer *l) {
  int32_t position = l->position;
  while (is_digit(l->ch)) {
    read_char(l);
  }

  return substr_range(&l->input, position, l->position - position);
}

void skip_whitespace(Lexer *l) {
  while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
    read_char(l);
  }
}

Lexer *Lexer_new(String input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  assert(lexer != NULL);

  lexer->input = input;
  lexer->position = 0;
  lexer->read_position = 0;
  lexer->ch = '\0';

  read_char(lexer);
  return lexer;
}

Token Token_from_char(TokenType type, char ch) {
  return (Token){type, str_from_char(ch)};
}
TokenType lookup_ident(String *literal) {
  for (size_t i = 0; i < sizeof(keywords_map) / sizeof(KeywordsMap); i++) {
    if (strcmp(literal->chars, keywords_map[i].literal) == 0) {
      return keywords_map[i].type;
    }
  }
  return IDENT;
}

Token next_token(Lexer *l) {
  Token t;

  skip_whitespace(l);
  switch (l->ch) {
  case '=':
    if (peek_char(l) == '=') {
      read_char(l);
      t = (Token){EQ, String_from("==")};
    } else
      t = Token_from_char(ASSIGN, l->ch);
    break;
  case '+':
    t = Token_from_char(PLUS, l->ch);
    break;
  case '-':
    t = Token_from_char(MINUS, l->ch);
    break;
  case ',':
    t = Token_from_char(COMMA, l->ch);
    break;
  case '/':
    t = Token_from_char(SLASH, l->ch);
    break;
  case '*':
    t = Token_from_char(ASTERISK, l->ch);
    break;
  case '<':
    t = Token_from_char(LT, l->ch);
    break;
  case '>':
    t = Token_from_char(GT, l->ch);
    break;
  case '!':
    if (peek_char(l) == '=') {
      read_char(l);
      t = (Token){NOT_EQ, String_from("!=")};
    } else {
      t = Token_from_char(BANG, l->ch);
    }
    break;
  case '(':
    t = Token_from_char(LPAREN, l->ch);
    break;
  case ')':
    t = Token_from_char(RPAREN, l->ch);
    break;
  case '{':
    t = Token_from_char(LBRACE, l->ch);
    break;
  case '}':
    t = Token_from_char(RBRACE, l->ch);
    break;
  case ';':
    t = Token_from_char(SEMICOLON, l->ch);
    break;
  default:
    if (is_letter(l->ch)) {
      t.literal = read_ident(l);
      t.type = lookup_ident(&t.literal);
      return t;
    } else if (is_digit(l->ch)) {
      t.literal = read_number(l);
      t.type = INT;
      return t;
    } else {
      t = Token_from_char(ILLEGAL, l->ch);
    }
    break;
  }

  read_char(l);
  return t;
}

void free_token(Token *t) {
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
