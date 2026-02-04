#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
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
String ident_token_literal(const Node *self) {
  Identifier *ident = (Identifier *)self;
  assert(ident != NULL);

  return String_clone(&ident->token.literal);
}

String ident_string(const Node *self) {
  Identifier *ident = (Identifier *)self;

  return ident->value;
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
String let_statement_token_literal(const Node *self) {
  LetStatement *let_st = (LetStatement *)self;

  return String_clone(&let_st->token.literal);
}

String let_statement_string(const Node *self) {
  LetStatement *let_st = (LetStatement *)self;

  String value;
  if (let_st->value != NULL) {
    String lt_val = let_st->value->vt->string((Node *)let_st->value);
    value = String_clone(&lt_val);
  } else {
    value = String_from("");
  }
  String name_str = let_st->name->base.vt->token_literal((Node *)let_st->name);
  String eq_str = String_from(" = ");
  String semicolon = String_from(";");
  String out = String_join(5, &let_st->token.literal, &name_str, &eq_str,
                           &value, &semicolon);

  free_string(&value);
  free_string(&name_str);
  free_string(&eq_str);
  free_string(&semicolon);
  return out;
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

String operator_expr_token_literal(const Node *self) {
  assert(self != NULL);
  OperatorExpr *op_expr = (OperatorExpr *)self;

  return op_expr->op.literal;
}

String operator_expr_string(const Node *self) { NOT_IMPLEMENTED("") }

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

IntExpr *int_expr_new(int value) {
  IntExpr *int_expr = malloc(sizeof(IntExpr));
  assert(int_expr != NULL);

  int_expr->base.vt = &INT_EXPR_VT;
  int_expr->value = value;

  return int_expr;
}
String int_expr_token_literal(const Node *self) {
  IntExpr *int_expr = (IntExpr *)self;
  char buf[INT_STR_MAX];
  sprintf(buf, "%d\n", int_expr->value);
  return String_from(buf);
}

String int_expr_string(const Node *self) { return String_from(""); }
void int_expr_destroy(Node *self) {
  IntExpr *int_expr = (IntExpr *)self;
  assert(int_expr != NULL);
  free(int_expr);
#ifdef DEBUG_PRINTS
  printf("IntExpr Destroyed\n");
#endif /* ifdef DEBUG_PRINTS */
}

ReturnStatement *return_st_new(const Expression *value) {
  ReturnStatement *ret_st = malloc(sizeof(ReturnStatement));

  assert(ret_st != NULL);

  ret_st->base.vt = &RETURN_ST_VT;
  ret_st->token = (Token){RETURN, String_from("return")};
  ret_st->value = value;

  return ret_st;
}
String return_st_token_literal(const Node *self) {
  assert(self != NULL);
  ReturnStatement *ret_st = (ReturnStatement *)self;
  assert(ret_st != NULL);
  return ret_st->token.literal;
}

String return_st_string(const Node *self) {

  assert(self != NULL);

  ReturnStatement *ret_st = (ReturnStatement *)self;

  String out = String_join(
      4, ret_st->base.vt->token_literal((Node *)ret_st), String_from(" "),
      ret_st->value->vt->string(ret_st->value), String_from(";"));

  return out;
}
void return_st_destroy(Node *self) {
  assert(self != NULL);
  ReturnStatement *ret_st = (ReturnStatement *)self;
  assert(ret_st != NULL);

  if (ret_st->value != NULL) {
    ret_st->value->vt->destroy((Node *)ret_st->value);
  }
  free_token(&ret_st->token);
  free(ret_st);
  printf("Return Statement destroyed\n");
}

StatementExpression *expr_st_new(const Token t, const Expression *expr) {
  StatementExpression *expr_st = malloc(sizeof(StatementExpression));
  expr_st->base.vt = &EXPR_ST_VT;
  expr_st->token = t;
  expr_st->expr = expr;
  return expr_st;
}
String expr_st_token_literal(const Node *self) {
  assert(self != NULL);
  StatementExpression *expr_st = (StatementExpression *)self;
  assert(expr_st != NULL);

  return expr_st->token.literal;
}
String expr_st_string(const Node *self) {
  assert(self != NULL);

  return self->vt->_t == EXPRESSION ? self->vt->string(self) : String_from("");
}

void expr_st_destroy(Node *self) {
  StatementExpression *st_expr = (StatementExpression *)self;
  assert(st_expr != NULL);

  free_token(&st_expr->token);
  st_expr->expr->vt->destroy((Node *)st_expr->expr);
  free(st_expr);
}
