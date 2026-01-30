#ifndef AST_H
#define AST_H

#include "lexer.h"

#include "utils.h"

typedef enum NodeType { EXPRESSION, STATEMENT } NodeType;

typedef struct Node Node;

typedef struct NodeVT {
  NodeType _t;
  String (*token_literal)(Node *self);
  void (*destroy)(Node *self);
} NodeVT;

// Abstract Node super-class/parent
struct Node {
  const NodeVT *vt;
};

// Expression Interface inherting Node
// vt._t = EXPRESSION
// DO NOT DIRECTLY USE IT
typedef Node Expression;

// Statement Interface inherting Node
// vt._t = STATEMENT
// DO NOT DIRECTLY USE IT
typedef Node Statement;

typedef struct Identifier {
  Expression base; // base.vt->_t = EXPRESSION
  Token token;
  String value;
} Identifier;

Identifier *ident_new(Token, String value);
String ident_token_literal(Node *self);
void ident_destroy(Node *self);

static const NodeVT IDENTIFIER_VT = {
    ._t = EXPRESSION,
    .token_literal = ident_token_literal,
    .destroy = ident_destroy,
};

typedef struct LetStatement {
  Statement base; // base.vt->_t = STATEMENT
  Token token;
  Identifier *name;
  Expression *value;
} LetStatement;

LetStatement *let_statement_new(Token, Identifier *, Expression *);
String let_statement_token_literal(Node *self);
void let_statement_destroy(Node *self);

static const NodeVT LET_STATEMENT_VT = {
    ._t = STATEMENT,
    .token_literal = let_statement_token_literal,
    .destroy = let_statement_destroy,
};

typedef struct OperatorExpression {
  Expression base;
  Expression *left;
  Token op;
  Expression *right;
} OperatorExpr;

OperatorExpr *operator_expr_new(Expression *left, Token *op, Expression *right);
String operator_expr_token_literal(Node *self);
void operator_expr_destroy(Node *self);

static const NodeVT OPERATOR_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = operator_expr_token_literal,
    .destroy = operator_expr_destroy,
};

typedef struct IntExpr {
  Expression base;
  int value;
} IntExpr;

IntExpr *int_expr_new(int value);
String int_expr_token_literal(Node *self);
void int_expr_destroy(Node *self);

static const NodeVT INT_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = int_expr_token_literal,
    .destroy = int_expr_destroy,
};

#endif // !AST_H
