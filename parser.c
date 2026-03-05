#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

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
  case TOKEN_LPAREN:
    return PREC_FN_CALL;

  default:
    return PREC_INVALID;
  }
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

  register_prefix(p, TOKEN_FUNCTION, (PrefixParseFn)parse_func_expression);
  register_prefix(p, TOKEN_IDENT, (PrefixParseFn)parse_identifier);
  register_prefix(p, TOKEN_IF, (PrefixParseFn)parse_if_expression);
  register_prefix(p, TOKEN_INT, (PrefixParseFn)parse_int_expr);
  register_prefix(p, TOKEN_LPAREN, (PrefixParseFn)parse_grouped_expression);
  register_prefix(p, TOKEN_TRUE, (PrefixParseFn)parse_boolean_expression);
  register_prefix(p, TOKEN_FALSE, (PrefixParseFn)parse_boolean_expression);
  register_prefix(p, TOKEN_BANG, (PrefixParseFn)parse_prefix_expression);
  register_prefix(p, TOKEN_MINUS, (PrefixParseFn)parse_prefix_expression);
  register_prefix(p, TOKEN_PLUS, (PrefixParseFn)parse_prefix_expression);

  register_infix(p, TOKEN_LPAREN, (InfixParseFn)parse_call_expression);
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
      token_type_to_string(tt), token_type_to_string(self->peek_token.type));
  string_array_push(&self->errors, String_from(buf));
}

void push_error(Parser *self, String message) {
  string_array_push(&self->errors, message);
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
    push_error(self, String_from("Parser error: Expected TOKEN_IDENT"));
    return NULL;
  }

  let_st->name = ident_new(Token_clone(&self->curr_token),
                           String_clone(&self->curr_token.literal));

  if (!expect_peek(self, TOKEN_ASSIGN)) {
    push_error(self, String_from("Parse error: expected TOKEN_ASSIGN"));

    return NULL;
  }

  parser_next_token(self);

  let_st->value = parse_expression(self, PREC_LOWEST);

  if (is_parser_peek_token(self, TOKEN_SEMICOLON)) {
    parser_next_token(self);
  }

  return let_st;
}
Identifier *parse_identifier(Parser *self) {
  assert(self != NULL);

  Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                String_clone(&self->curr_token.literal));

  if (ident == NULL) {
    return NULL;
  }

  return ident;
}

Statement *parse_if_statement(Parser *self) { return NULL; }
ReturnStatement *parse_return_statement(Parser *self) {

  ReturnStatement *ret_st = return_st_new(Token_clone(&self->curr_token), NULL);

  parser_next_token(self);

  ret_st->value = parse_expression(self, PREC_LOWEST);

  if (is_parser_peek_token(self, TOKEN_SEMICOLON)) {
    parser_next_token(self);
  }

  return ret_st;
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

    InfixParseFn infix = self->infix_parse_fns[self->peek_token.type];
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

  infix_new->right = parse_expression(self, prec);

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
  IntExpr *int_expr = int_expr_new(Token_clone(&self->curr_token), value);

  return int_expr;
}

Expression *parse_grouped_expression(Parser *self) {
  assert(self != NULL);

  parser_next_token(self);

  Expression *expr = parse_expression(self, PREC_LOWEST);

  if (!expect_peek(self, TOKEN_RPAREN)) {

    return NULL;
  }

  return expr;
}

BooleanExpression *parse_boolean_expression(Parser *self) {
  assert(self != NULL);

  return bool_expr_new(Token_clone(&self->curr_token),
                       is_parser_curr_token(self, TOKEN_TRUE));
}

IfExpression *parse_if_expression(Parser *self) {
  assert(self != NULL);
  IfExpression *if_expr =
      if_expr_new(Token_clone(&self->curr_token), NULL, NULL, NULL);

  if (!expect_peek(self, TOKEN_LPAREN)) {
    return NULL;
  }

  parser_next_token(self);
  if_expr->condition = parse_expression(self, PREC_LOWEST);
  if (!expect_peek(self, TOKEN_RPAREN)) {
    return NULL;
  }

  if (!expect_peek(self, TOKEN_LBRACE)) {
    return NULL;
  }

  if_expr->consequence = parse_block_statement(self);

  if (is_parser_peek_token(self, TOKEN_ELSE)) {
    parser_next_token(self);
    if (!expect_peek(self, TOKEN_LBRACE)) {
      return NULL;
    }

    if_expr->alternative = parse_block_statement(self);
  }

  return if_expr;
}

BlockStatement *parse_block_statement(Parser *self) {
  assert(self != NULL);
  BlockStatement *block_st = block_statement_new(Token_clone(&self->curr_token),
                                                 statements_array_init(1));

  parser_next_token(self);

  while (!is_parser_curr_token(self, TOKEN_RBRACE) &&
         !is_parser_curr_token(self, TOKEN_EOF)) {
    Statement *stmt = parse_statement(self);
    if (stmt != NULL)
      statements_push(&block_st->statements, stmt);

    parser_next_token(self);
  }

  return block_st;
}

IdentifiersArray parse_func_parameters(Parser *self) {
  assert(self != NULL);
  IdentifiersArray ident_arr = identifiers_array_init(0);
  if (is_parser_peek_token(self, TOKEN_RPAREN)) {
    parser_next_token(self);
    return ident_arr;
  }

  parser_next_token(self);

  Identifier *first_param = ident_new(Token_clone(&self->curr_token),
                                      String_clone(&self->curr_token.literal));

  identifiers_push(&ident_arr, first_param);

  while (is_parser_peek_token(self, TOKEN_COMMA)) {
    parser_next_token(self); // set the current token to COMMA (,)
    parser_next_token(self); // now set it to the identifier
    Identifier *ident = ident_new(Token_clone(&self->curr_token),
                                  String_clone(&self->curr_token.literal));
    identifiers_push(&ident_arr, ident);
  }
  if (!expect_peek(self, TOKEN_RPAREN)) {
    free_identifiers(&ident_arr);
    push_error(
        self,
        String_from("Error parsing parameter list: expected TOKEN_RPAREN"));
    return identifiers_array_init(0);
  }

  return ident_arr;
}

FnExpression *parse_func_expression(Parser *self) {
  assert(self != NULL);
  FnExpression *fn_expr = fn_expr_new(Token_clone(&self->curr_token),
                                      identifiers_array_init(0), NULL);

  if (!expect_peek(self, TOKEN_LPAREN)) {
    return NULL;
  }

  // we remove any allocation done here
  free_identifiers(&fn_expr->parameters);

  fn_expr->parameters = parse_func_parameters(self);

  if (!expect_peek(self, TOKEN_LBRACE)) {
    return NULL;
  }

  fn_expr->body = parse_block_statement(self);

  return fn_expr;
}

CallExpression *parse_call_expression(Parser *self, Expression *left) {
  assert(self != NULL);
  assert(left != NULL);
  CallExpression *call_expr = call_expr_new(Token_clone(&self->curr_token),
                                            left, expressions_array_init(0));

  free_expressions(&call_expr->arguments);

  call_expr->arguments = parse_argument_list(self);

  return call_expr;
}

ExpressionsArray parse_argument_list(Parser *self) {
  assert(self != NULL);
  if (is_parser_peek_token(self, TOKEN_RPAREN)) {
    parser_next_token(self);
    return expressions_array_init(0);
  }
  ExpressionsArray expr_arr = expressions_array_init(0);

  // move to the first arguement expr token
  parser_next_token(self);

  // push the first argument expression
  expressions_push(&expr_arr, parse_expression(self, PREC_LOWEST));
  while (is_parser_peek_token(self, TOKEN_COMMA)) {
    parser_next_token(self);
    parser_next_token(self);
    expressions_push(&expr_arr, parse_expression(self, PREC_LOWEST));
  }

  if (!expect_peek(self, TOKEN_RPAREN)) {
    push_error(self, String_from("Parser error: expected "
                                 "TOKEN_RPAREN"));
    free_expressions(&expr_arr);
    return expressions_array_init(0);
  }

  return expr_arr;
}
