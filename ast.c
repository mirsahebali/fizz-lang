#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "utils.h"
#define INT_STR_MAX 20

Identifier *ident_new(Token token, String value) {
  Identifier *ident = malloc(sizeof(Identifier));
  assert(ident != NULL);

  ident->base.vt = &IDENTIFIER_VT;
  ident->token = token;
  ident->value = value;

  return ident;
}
String ident_token_literal(Node *self) {
  Identifier *ident = (Identifier *)self;
  assert(ident != NULL);

  return str_clone(&ident->token.literal);
}
void ident_destroy(Node *self) {
  if (self == NULL)
    return;

  Identifier *ident = (Identifier *)self;

  if (ident == NULL)
    return;

  free_token(&ident->token);
  free_string(&ident->value);
  free(ident);

  printf("Identifier destroyed\n");
}

LetStatement *let_statement_new(Token token, Identifier *name,
                                Expression *value) {

  LetStatement *let_st = malloc(sizeof(LetStatement));
  assert(let_st != NULL);

  let_st->base.vt = &LET_STATEMENT_VT;
  let_st->token = token;
  let_st->name = name;
  let_st->value = value;

  return let_st;
}
String let_statement_token_literal(Node *self) {
  LetStatement *let_st = (LetStatement *)self;

  return str_clone(&let_st->token.literal);
}
void let_statement_destroy(Node *self) {
  LetStatement *let_st = (LetStatement *)self;
  assert(let_st != NULL);
  if (let_st->name != NULL) {
    let_st->name->base.vt->destroy((Node *)let_st->name);
  }
  if (let_st->value != NULL) {
    let_st->value->vt->destroy((Node *)let_st->value);
  }
  free(let_st);
  printf("LetStatement destroyed\n");
}

String operator_expr_token_literal(Node *self) { return String_from(""); }
void operator_expr_destroy(Node *self) {
  if (self == NULL)
    return;
  OperatorExpr *op_expr = (OperatorExpr *)self;
  assert(op_expr != NULL);
  op_expr->left->vt->destroy(op_expr->left);
  op_expr->right->vt->destroy(op_expr->right);
  free_token(&op_expr->op);
  free(op_expr);

#ifdef DEBUG_PRINTS
  printf("OprExpr destroyed \n");
#endif /* ifdef DEBUG_PRINTS */
}

OperatorExpr *operator_expr_new(Expression *left, Token *op,
                                Expression *right) {
  OperatorExpr *op_expr = (OperatorExpr *)malloc(sizeof(OperatorExpr));
  op_expr->base.vt = &OPERATOR_EXPR_VT;
  if (left->vt->_t != EXPRESSION) {
    printf("Left value is not an expression\n");
    assert(left->vt->_t != EXPRESSION);
  };

  if (right->vt->_t != EXPRESSION) {
    printf("Right value is not an expression\n");
    assert(right->vt->_t != EXPRESSION);
  };

  if (op->type != PLUS || op->type != MINUS || op->type != ASTERISK ||
      op->type != SLASH) {
    printf("Invalid op: %s\n", token_type_to_string(op->type));
    assert(op->type != PLUS || op->type != MINUS || op->type != ASTERISK ||
           op->type != SLASH);
  }

  op_expr->left = left;
  op_expr->op = Token_clone(op);
  op_expr->right = right;

  return op_expr;
}

IntExpr *int_expr_new(int value) {
  IntExpr *int_expr = malloc(sizeof(IntExpr));
  assert(int_expr != NULL);

  int_expr->base.vt = &INT_EXPR_VT;
  int_expr->value = value;

  return int_expr;
}
String int_expr_token_literal(Node *self) {
  IntExpr *int_expr = (IntExpr *)self;
  char buf[INT_STR_MAX];
  sprintf(buf, "%d\n", int_expr->value);
  return String_from(buf);
}
void int_expr_destroy(Node *self) {
  IntExpr *int_expr = (IntExpr *)self;
  assert(int_expr != NULL);
  free(int_expr);
#ifdef DEBUG_PRINTS
  printf("IntExpr Destroyed\n");
#endif /* ifdef DEBUG_PRINTS */
}
