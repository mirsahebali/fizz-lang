#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

#define ERROR_STRING_MAX 100

StatementsArray statements_array_init(int32_t capacity) {

  assert(capacity >= 0);
  StatementsArray s;

  s.size = 0;
  s.capacity = capacity == 0 ? 16 : capacity;
  s.data = malloc(sizeof(Statement *) * capacity);
  assert(s.data != NULL);
  return s;
}

bool statements_reserve(StatementsArray *s, int32_t new_capacity) {
  assert(s != NULL);
  if (new_capacity <= s->capacity)
    return true;
  Statement **new_ptr = realloc(s->data, sizeof(Statement *) * new_capacity);

  if (!new_ptr)
    return false;

  s->data = new_ptr;
  s->capacity = new_capacity;
  return true;
}
// realloc the arena and push the data
int32_t statements_push(StatementsArray *s, Statement *data) {
  assert(s != NULL);
  if (s->capacity == s->size) {
    int32_t new_cap = s->capacity > 0 ? s->capacity * 2 : 1;
    assert(statements_reserve(s, new_cap));
  }

  s->data[s->size] = data;
  return ++s->size;
}

Statement *statements_get(StatementsArray *s, int32_t index) {
  assert(s != NULL);
  assert(index >= 0 && index < s->size);

  return s->data[index];
}

int32_t statements_size(StatementsArray *s) {
  assert(s != NULL);
  return s->size;
}
int32_t statements_capacity(StatementsArray *s) {
  assert(s != NULL);
  return s->capacity;
}
void free_statements(StatementsArray *s) {
  if (s == NULL)
    return;

  for (int i = 0; i < s->size; i++) {
    s->data[i]->vt->destroy(s->data[i]);
  }

  free(s->data);
  s->size = 0;
  s->capacity = 0;
}

String token_literal(Program *p) {
  assert(p != NULL);
  if (statements_size(&p->statements) > 0) {
    Statement *s = statements_get(&p->statements, 0);
    return s->vt->token_literal(s);
  }

  return String_from("");
}

String program_string(Program *self) {
  StringArray arr = string_array_init(4);
  String out;

  for (int32_t i = 0; i < self->statements.size; i++) {
  }

  return out;
}

Program *Program_new(StatementsArray st_array) {
  Program *p = malloc(sizeof(Program));
  p->statements = statements_array_init(10);

  return p;
}

void free_program(Program *p) {
  free_statements(&p->statements);
  free(p);
}

Parser *Parser_new(Lexer *l) {
  assert(l != NULL);
  Parser *p = (Parser *)malloc(sizeof(Parser));
  p->lexer = l;
  p->peek_token = (Token){ILLEGAL, String_from("")};
  p->curr_token = (Token){ILLEGAL, String_from("")};
  p->errors = string_array_init(1);
  parser_next_token(p);
  parser_next_token(p);
  return p;
}

const StringArray *parser_errors(const Parser *self) { return &self->errors; }

void peek_error(Parser *self, const TokenType tt) {
  char buf[ERROR_STRING_MAX];
  snprintf(
      buf, ERROR_STRING_MAX, "expected next token to be %s, got %s instead",
      token_type_to_string(tt), token_type_to_string(self->curr_token.type));
  string_array_push(&self->errors, String_from(buf));
}

void free_parser(Parser *p) {
  if (p == NULL)
    return;

  free_string_array(&p->errors);
  free_token(&p->peek_token);
  free_token(&p->curr_token);
  free_lexer(p->lexer);
  free(p);
}

void parser_next_token(Parser *self) {
  assert(self != NULL);

  free_token(&self->curr_token);
  self->curr_token = Token_clone(&self->peek_token);

  free_token(&self->peek_token);
  self->peek_token = next_token(self->lexer);
}

Statement *parse_statement(Parser *self) {
  switch (self->curr_token.type) {
  case LET:
    return (Statement *)parse_let_statement(self);
  case RETURN:
    return (Statement *)parse_return_statement(self);
  default:
    return NULL;
  }
}

bool is_parser_curr_token(const Parser *self, const TokenType t) {
  return self->curr_token.type == t;
}

bool is_parser_peek_token(const Parser *self, const TokenType t) {
  return self->peek_token.type == t;
}

bool expect_peek(Parser *self, const TokenType t) {
  if (is_parser_peek_token(self, t)) {
    parser_next_token(self);
    return true;
  }
  peek_error(self, t);
  return false;
}

Program *parse_program(Parser *self) {
  assert(self != NULL);
  Program *program = (Program *)malloc(sizeof(Program));
  program->statements = statements_array_init(10);

  assert(program != NULL);
  parser_next_token(self);

  int count = 0;
  while (!is_parser_curr_token(self, EOF_T)) {
    Statement *st = parse_statement(self);

    if (st != NULL) {
      statements_push(&program->statements, st);
    }
    parser_next_token(self);
  }
  return program;
}

void print_errors(Parser *self) { print_string_array(&self->errors); }

LetStatement *parse_let_statement(Parser *self) {
  assert(self != NULL);

  LetStatement *let_st = let_statement_new(self->curr_token, NULL, NULL);

  if (!expect_peek(self, IDENT)) {
    return NULL;
  }

  Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                String_clone(&self->curr_token.literal));
  assert(ident != NULL);

  let_st->name = ident;
  if (!expect_peek(self, ASSIGN)) {
    printf("Parse error: no equal sign\n");
    return NULL;
  }

  // TODO: skipping handling rvalue expression
  while (!is_parser_curr_token(self, SEMICOLON)) {
    parser_next_token(self);
  }

  // Expression *expr = parse_expression(self);

  return let_st;
}
Identifier *parse_identifier(Parser *self) {
  Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                String_clone(&self->curr_token.literal));

  assert(ident != NULL);
  return ident;
}
Statement *parse_if_statement(Parser *self) { return NULL; }
ReturnStatement *parse_return_statement(Parser *self) {
  ReturnStatement *ret_st = return_st_new(NULL);
  parser_next_token(self);

  while (!is_parser_curr_token(self, SEMICOLON)) {
    parser_next_token(self);
  }
  return ret_st;
}
Expression *parse_expression(Parser *self) {
  if (self->curr_token.type == INT) {
    if (self->peek_token.type == PLUS) {
      return (Expression *)parse_operator_expr(self);
    } else if (self->peek_token.type == SEMICOLON) {
      return (Expression *)parse_integer_literal(self);
    }
  } else if (self->curr_token.type == LPAREN) {
    return (Expression *)parse_grouped_expr(self);
  }
  // TODO: add more parsing functions and checks
  return NULL;
}
OperatorExpr *parse_operator_expr(Parser *self) {
  OperatorExpr *op_expr = (OperatorExpr *)malloc(sizeof(OperatorExpr));

  assert(op_expr != NULL);

  op_expr->left = parse_integer_literal(self);
  parser_next_token(self);
  op_expr->op = Token_clone(&self->curr_token);
  parser_next_token(self);
  op_expr->right = parse_expression(self);

  return op_expr;
}

Expression *parse_integer_literal(Parser *self) { return NULL; }
Expression *parse_grouped_expr(Parser *self) { return NULL; }
