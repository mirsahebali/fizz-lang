#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer.h"
#include "utils.h"

#include "cstring.h/cstring.h"

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
  free(self);

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
  String spc_str = String_from(" ");
  String semicolon = String_from(";");
  String *out = String_join(6, &let_st->token.literal, &spc_str, &name_str,
                            &eq_str, &value, &semicolon);

  String ret = String_clone(out);

  free_string(out);
  free_string(&value);
  free_string(&name_str);
  free_string(&eq_str);
  free_string(&semicolon);
  free_string(&spc_str);

  free(out);

  return ret;
}

void let_statement_destroy(Node *self) {
  if (self == NULL)
    return;
  LetStatement *let_st = (LetStatement *)self;

  free_token(&let_st->token);
  if (let_st->name != NULL) {
    let_st->name->base.vt->destroy((Node *)let_st->name);
  }
  if (let_st->value != NULL) {
    let_st->value->vt->destroy((Node *)let_st->value);
  }

  printf("LetStatement destroyed\n");

  free(self);
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

  bool is_invalid_op = op->type != TOKEN_PLUS && op->type != TOKEN_MINUS &&
                       op->type != TOKEN_ASTERISK && op->type != TOKEN_SLASH;
  if (is_invalid_op) {
    printf("Invalid op: %s\n", token_type_to_string(op->type));
    assert(is_invalid_op);
  }

  op_expr->left = left;
  op_expr->op = Token_clone(op);
  op_expr->right = right;

  return op_expr;
}

String operator_expr_token_literal(const Node *self) {
  assert(self != NULL);
  OperatorExpr *op_expr = (OperatorExpr *)self;

  return String_clone(&op_expr->op.literal);
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
  free(self);

  printf("OprExpr destroyed \n");
}

IntExpr *int_expr_new(const int value) {
  IntExpr *int_expr = malloc(sizeof(IntExpr));
  assert(int_expr != NULL);

  int_expr->base.vt = &INT_EXPR_VT;
  int_expr->token = (Token){TOKEN_INT, String_from_int(value)};
  int_expr->value = value;

  return int_expr;
}
String int_expr_token_literal(const Node *self) {
  assert(self != NULL);
  IntExpr *int_expr = (IntExpr *)self;
  return String_clone(&int_expr->token.literal);
};

String int_expr_string(const Node *self) {
  assert(self != NULL);

  IntExpr *int_expr = (IntExpr *)self;

  return String_clone(&int_expr->token.literal);
}
void int_expr_destroy(Node *self) {
  IntExpr *int_expr = (IntExpr *)self;
  assert(int_expr != NULL);
  free_token(&int_expr->token);
  printf("IntExpr Destroyed\n");
  free(self);
}

ReturnStatement *return_st_new(const Expression *value) {
  ReturnStatement *ret_st = malloc(sizeof(ReturnStatement));

  assert(ret_st != NULL);

  ret_st->base.vt = &RETURN_ST_VT;
  ret_st->token = (Token){TOKEN_RETURN, String_from("return")};
  ret_st->value = value;

  return ret_st;
}
String return_st_token_literal(const Node *self) {
  assert(self != NULL);
  ReturnStatement *ret_st = (ReturnStatement *)self;

  assert(ret_st != NULL);
  return String_clone(&ret_st->token.literal);
}

String return_st_string(const Node *self) {

  assert(self != NULL);

  ReturnStatement *ret_st = (ReturnStatement *)self;

  String *out = String_join(
      4, ret_st->base.vt->token_literal((Node *)ret_st), String_from(" "),
      ret_st->value->vt->string(ret_st->value), String_from(";"));

  String ret = String_clone(out);
  free_string(out);
  free(out);

  return ret;
}
void return_st_destroy(Node *self) {
  assert(self != NULL);
  ReturnStatement *ret_st = (ReturnStatement *)self;
  assert(ret_st != NULL);

  if (ret_st->value != NULL) {
    ret_st->value->vt->destroy((Node *)ret_st->value);
  }
  free_token(&ret_st->token);
  free(self);
  printf("Return Statement destroyed\n");
}

ExpressionStatement *expr_st_new(const Token t, const Expression *expr) {
  ExpressionStatement *expr_st = malloc(sizeof(ExpressionStatement));
  expr_st->base.vt = &EXPR_ST_VT;
  expr_st->token = t;
  expr_st->expr = expr;
  return expr_st;
}
String expr_st_token_literal(const Node *self) {
  assert(self != NULL);
  ExpressionStatement *expr_st = (ExpressionStatement *)self;
  assert(expr_st != NULL);

  return expr_st->token.literal;
}
String expr_st_string(const Node *self) {
  assert(self != NULL);

  return self->vt->_t == EXPRESSION ? self->vt->string(self) : String_from("");
}

void expr_st_destroy(Node *self) {
  ExpressionStatement *st_expr = (ExpressionStatement *)self;
  assert(st_expr != NULL);

  free_token(&st_expr->token);
  st_expr->expr->vt->destroy((Node *)st_expr->expr);
  free(self);
}

PrefixExpression *prefix_expr_new(const Token t, const String op,
                                  Expression *right) {
  PrefixExpression *prefix_expr = malloc(sizeof(PrefixExpression));

  prefix_expr->base.vt = &PREFIX_EXPR_VT;
  prefix_expr->right = right;
  prefix_expr->token = Token_clone(&t);
  prefix_expr->op = op;

  return prefix_expr;
}
String prefix_expr_token_literal(const Node *self) {
  assert(self != NULL);

  PrefixExpression *prefix_expr = (PrefixExpression *)self;
  return String_clone(&prefix_expr->token.literal);
}
String prefix_expr_string(const Node *self) {
  assert(self != NULL);

  PrefixExpression *prefix_expr = (PrefixExpression *)self;
  String right_expr_str = (prefix_expr->right->vt->string(prefix_expr->right));
  String left_paren = STR_NEW("(");
  String right_paren = STR_NEW(")");
  String *out_str = String_join(4, &left_paren, &prefix_expr->op,
                                &right_expr_str, &right_paren);

  String out = String_clone(out_str);

  free_string(out_str);
  free_string(&right_expr_str);

  return out;
}
void prefix_expr_destroy(Node *self) {

  assert(self != NULL);

  PrefixExpression *prefix_expr = (PrefixExpression *)self;

  free_token(&prefix_expr->token);
  free_string(&prefix_expr->op);
  prefix_expr->right->vt->destroy((Node *)prefix_expr->right);

  free(self);
}

InfixExpression *infix_expr_new(const Token t, Expression *left,
                                const String op, Expression *right) {
  InfixExpression *infix_expr =
      (InfixExpression *)malloc(sizeof(InfixExpression));

  infix_expr->base.vt = &INFIX_EXPR_VT;
  infix_expr->token = t;
  infix_expr->left = left;
  infix_expr->right = right;
  infix_expr->op = op;

  return infix_expr;
}
String infix_expr_token_literal(const Node *self) {
  InfixExpression *infix_expr = (InfixExpression *)self;
  return String_clone(&infix_expr->token.literal);
}
String infix_expr_string(const Node *self) {

  InfixExpression *infix_expr = (InfixExpression *)self;
  String l_paren = STR_NEW("(");
  String r_paren = STR_NEW(")");
  String left_str = infix_expr->left->vt->string((Node *)infix_expr->left);
  String right_str = infix_expr->right->vt->string((Node *)infix_expr->right);
  String *out_str = String_join(5, &l_paren, &left_str, &infix_expr->op,
                                &right_str, &r_paren);

  String out = String_clone(out_str);
  free_string(out_str);
  return out;
}
void infix_expr_destroy(Node *self) {
  InfixExpression *infix_expr = (InfixExpression *)self;

  if (infix_expr->right != NULL) {
    infix_expr->right->vt->destroy((Node *)infix_expr->right);
  }

  if (infix_expr->left != NULL) {
    infix_expr->left->vt->destroy((Node *)infix_expr->left);
  }

  free_string(&infix_expr->op);
  free_token(&infix_expr->token);

  free(self);
}
