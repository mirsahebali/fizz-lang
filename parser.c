#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

#define TRACE_BEGIN printf("\t\nBEGIN %s", __FUNCTION__);
#define TRACE_END printf("\t\nEND %s", __FUNCTION__);

#define ERROR_STRING_MAX 100

Precedence precedence_map(TokenType tt) {
  switch (tt) {
  case TOKEN_EQ:
    return PREC_EQUALS;
  case TOKEN_NOT_EQ:
    return PREC_EQUALS;
  case TOKEN_LT:
    return PREC_LESSGREATER;
  case TOKEN_GT:
    return PREC_LESSGREATER;
  case TOKEN_PLUS:
    return PREC_SUM;
  case TOKEN_MINUS:
    return PREC_SUM;
  case TOKEN_SLASH:
    return PREC_PRODUCT;
  case TOKEN_ASTERISK:
    return PREC_PRODUCT;

  default:
    return PREC_INVALID;
  }
}

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
  assert(index >= 0);
  assert(index < s->size);

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
    s->data[i]->vt->destroy((Node *)s->data[i]);
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

  for (int32_t i = 0; i < self->statements.size; i++) {
    Statement *st = statements_get(&self->statements, i);
    if (st == NULL)
      return STR_NULL;

    String str = st->vt->string((Node *)st);
    string_array_push(&arr, str);
  }

  String out = string_array_join(&arr, STR_NEW(" "));
  free_string_array(&arr);

  return out;
}

Program *Program_new(StatementsArray st_array) {
  Program *p = malloc(sizeof(Program));
  p->statements = st_array;

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
  p->peek_token = (Token){TOKEN_ILLEGAL, String_from("")};
  p->curr_token = (Token){TOKEN_ILLEGAL, String_from("")};
  p->errors = string_array_init(1);
  for (size_t i = 0; i < TOKEN_COUNT; i++) {
    p->prefix_parse_fns[i] = NULL;
    p->infix_parse_fns[i] = NULL;
  }
  register_prefix(p, TOKEN_IDENT, (PrefixParseFn)parse_identifier);
  register_prefix(p, TOKEN_INT, (PrefixParseFn)parse_int_expr);
  register_prefix(p, TOKEN_BANG, (PrefixParseFn)parse_prefix_expression);
  register_prefix(p, TOKEN_MINUS, (PrefixParseFn)parse_prefix_expression);
  register_prefix(p, TOKEN_PLUS, (PrefixParseFn)parse_prefix_expression);

  register_infix(p, TOKEN_IDENT, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_INT, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_PLUS, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_MINUS, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_NOT_EQ, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_EQ, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_ASTERISK, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_LT, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_GT, (InfixParseFn)parse_infix_expression);
  register_infix(p, TOKEN_SLASH, (InfixParseFn)parse_infix_expression);

  parser_next_token(p);
  parser_next_token(p);
  return p;
}

Precedence peek_precedence(Parser *self) {
  Precedence peek_prec = precedence_map(self->peek_token.type);
  if (peek_prec != PREC_INVALID) {
    return peek_prec;
  }
  return PREC_LOWEST;
}

Precedence curr_precedence(Parser *self) {
  Precedence curr_prec = precedence_map(self->curr_token.type);
  if (curr_prec != PREC_INVALID) {
    return curr_prec;
  }
  return PREC_LOWEST;
}

const StringArray *parser_errors(const Parser *self) { return &self->errors; }

void no_prefix_parse_error(Parser *self, const TokenType tt) {
  char buf[512];
  snprintf(buf, 512, "no prefix parse function for %s found",
           token_type_to_string(tt));

  string_array_push(&self->errors, String_from(buf));
};

void peek_error(Parser *self, const TokenType tt) {
  char buf[ERROR_STRING_MAX];
  snprintf(
      buf, ERROR_STRING_MAX, "expected next token to be %s, got %s instead",
      token_type_to_string(tt), token_type_to_string(self->curr_token.type));
  string_array_push(&self->errors, String_from(buf));
}

void register_prefix(Parser *self, TokenType tt, PrefixParseFn fn) {
  self->prefix_parse_fns[tt] = fn;
}
void register_infix(Parser *self, TokenType tt, InfixParseFn fn) {
  self->infix_parse_fns[tt] = fn;
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
  case TOKEN_LET:
    return (Statement *)parse_let_statement(self);
  case TOKEN_RETURN:
    return (Statement *)parse_return_statement(self);

  default:
    return (Statement *)parse_expression_statement(self);
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
  program->statements = statements_array_init(1);

  assert(program != NULL);
  parser_next_token(self);

  while (!is_parser_curr_token(self, TOKEN_EOF)) {
    Statement *st = parse_statement(self);

    if (st != NULL) {
      statements_push(&program->statements, st);
    }
    parser_next_token(self);
  }
  return program;
}

void print_errors(Parser *self) {
  if (self->errors.size == 0) {
    printf("No parser errors found(yet)\n");
    return;
  }
  print_string_array(&self->errors);
}

LetStatement *parse_let_statement(Parser *self) {
  assert(self != NULL);

  LetStatement *let_st =
      let_statement_new(Token_clone(&self->curr_token), NULL, NULL);

  if (!expect_peek(self, TOKEN_IDENT)) {
    return NULL;
  }

  Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                String_clone(&self->curr_token.literal));
  assert(ident != NULL);

  let_st->name = ident;
  if (!expect_peek(self, TOKEN_ASSIGN)) {
    printf("Parse error: no equal sign\n");
    return NULL;
  }

  // TODO: skipping handling rvalue expression
  while (!is_parser_curr_token(self, TOKEN_SEMICOLON)) {
    parser_next_token(self);
  }

  // Expression *expr = parse_expression(self);

  return let_st;
  TRACE_END;
}
Identifier *parse_identifier(Parser *self) {
  assert(self != NULL);

  Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                String_clone(&self->curr_token.literal));

  assert(ident != NULL);
  return ident;
  TRACE_END;
}

Statement *parse_if_statement(Parser *self) { return NULL; }
ReturnStatement *parse_return_statement(Parser *self) {
  ReturnStatement *ret_st = return_st_new(NULL);
  parser_next_token(self);

  while (!is_parser_curr_token(self, TOKEN_SEMICOLON)) {
    parser_next_token(self);
  }
  return ret_st;
  TRACE_END;
}
Expression *parse_expression(Parser *self, Precedence prec) {
  PrefixParseFn prefix = self->prefix_parse_fns[self->curr_token.type];

  if (prefix == NULL) {
    no_prefix_parse_error(self, self->curr_token.type);
    return NULL;
  }

  Expression *left_expr = prefix(self);

  while (!is_parser_peek_token(self, TOKEN_SEMICOLON) &&
         prec < peek_precedence(self)) {

    InfixParseFn infix = self->infix_parse_fns[self->curr_token.type];
    if (infix == NULL) {
      return left_expr;
    }

    parser_next_token(self);

    left_expr = infix(self, left_expr);
  }

  return left_expr;
}

PrefixExpression *parse_prefix_expression(Parser *self) {

  PrefixExpression *expr_new = prefix_expr_new(
      self->curr_token, String_clone(&self->curr_token.literal), NULL);

  parser_next_token(self);

  expr_new->right = parse_expression(self, PREC_PREFIX);

  return expr_new;
}

InfixExpression *parse_infix_expression(Parser *self, Expression *left) {

  InfixExpression *infix_new =
      infix_expr_new(Token_clone(&self->curr_token), left,
                     String_clone(&self->curr_token.literal), NULL);

  Precedence prec = curr_precedence(self);

  parser_next_token(self);
  // String plus = STR_NEW("+");
  // if (String_cmp(&infix_new->op, &plus)) {
  //   infix_new->right = parse_expression(self, prec - 1);
  // } else {
  infix_new->right = parse_expression(self, prec);
  // }

  return infix_new;
}
OperatorExpr *parse_operator_expr(Parser *self) {
  OperatorExpr *op_expr = (OperatorExpr *)malloc(sizeof(OperatorExpr));

  assert(op_expr != NULL);

  op_expr->left = (Expression *)parse_int_expr(self);
  parser_next_token(self);
  op_expr->op = Token_clone(&self->curr_token);
  parser_next_token(self);

  op_expr->right = parse_expression(self, PREC_FN_CALL);

  return op_expr;
}

ExpressionStatement *parse_expression_statement(Parser *self) {
  ExpressionStatement *stmt = expr_st_new(Token_clone(&self->curr_token), NULL);

  stmt->expr = parse_expression(self, PREC_LOWEST);

  if (is_parser_peek_token(self, TOKEN_SEMICOLON)) {
    parser_next_token(self);
  }
  return stmt;
}

IntExpr *parse_int_expr(Parser *self) {
  int32_t value;
  bool is_success = String_to_int(&self->curr_token.literal, &value);
  if (!is_success) {
    String error_str = String_join(
        2, String_from("ERROR: converting string to int for value: "),
        &self->curr_token.literal);
    string_array_push(&self->errors, error_str);
    return NULL;
  }
  IntExpr *int_expr = int_expr_new(value);
  return int_expr;
}
Expression *parse_grouped_expr(Parser *self) { return NULL; }
