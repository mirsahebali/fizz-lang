#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum NodeType { EXPRESSION, STATEMENT } NodeType;

typedef struct Node Node;

typedef struct NodeVT {
  NodeType _t;
  String (*token_literal)(const Node *self);
  String (*string)(const Node *self);
  void (*destroy)(Node *self);
} NodeVT;

// Abstract Node super-class/parent
struct Node {
  const NodeVT *vt;
};

// Expression Interface composing Node
// vt._t = EXPRESSION
// DO NOT DIRECTLY USE IT
typedef Node Expression;

// Statement Interface composing Node
// vt._t = STATEMENT
// DO NOT DIRECTLY USE IT
typedef Node Statement;

typedef struct Identifier Identifier;

typedef struct StatementsArray {
  int32_t capacity;
  int32_t size;
  // stores pointer to any inherted statment which implements those functions
  // each Statement* data can be of any intertied given type which we can use
  // for dynamic dispatch
  Statement **data;
} StatementsArray;

StatementsArray statements_array_init(int32_t size);
// realloc the arena and push the data
int32_t statements_push(StatementsArray *, Statement *data);

bool statements_reserve(StatementsArray *, int32_t new_capacity);
Statement *statements_get(StatementsArray *, int32_t index);
// realloc the arena and push the data
int32_t statements_size(StatementsArray *);
int32_t statements_capacity(StatementsArray *);
void free_statements(StatementsArray *);
// Statement Array impl end

typedef struct IdentifiersArray {
  int32_t capacity;
  int32_t size;

  // we store the list of reference to the identifers as they can be evaluated
  // to have any expression or value
  Identifier **data;
} IdentifiersArray;

IdentifiersArray identifiers_array_init(int32_t size);
// realloc the arena and push the data
int32_t identifiers_push(IdentifiersArray *, Identifier *data);

bool identifiers_reserve(IdentifiersArray *, int32_t new_capacity);
Identifier *identifiers_get(IdentifiersArray *, int32_t index);
// realloc the arena and push the data
int32_t identifiers_size(IdentifiersArray *);
int32_t identifiers_capacity(IdentifiersArray *);
void free_identifiers(IdentifiersArray *);
// Identifier Array impl end

// ExpressionArray impl start ---
typedef struct {
  int32_t capacity;
  int32_t size;

  Expression **data;
} ExpressionsArray;

ExpressionsArray expressions_array_init(int32_t size);
// realloc the arena and push the data
int32_t expressions_push(ExpressionsArray *, Expression *data);

bool expressions_reserve(ExpressionsArray *, int32_t new_capacity);
Expression *expressions_get(ExpressionsArray *, int32_t index);
// realloc the arena and push the data
int32_t expressions_size(ExpressionsArray *);
int32_t expressions_capacity(ExpressionsArray *);
void free_expressions(ExpressionsArray *);

// ExpressionArray impl end ---

struct Identifier {
  Expression base; // base.vt->_t = EXPRESSION
  Token token;
  String value;
};

Identifier *ident_new(Token, String value);
String ident_token_literal(const Node *self);
String ident_string(const Node *self);
void ident_destroy(Node *self);

static const NodeVT IDENTIFIER_VT = {
    ._t = EXPRESSION,
    .token_literal = ident_token_literal,
    .string = ident_string,
    .destroy = ident_destroy,
};

typedef struct LetStatement {
  Statement base; // base.vt->_t = STATEMENT
  Token token;
  Identifier *name;
  Expression *value;
} LetStatement;

LetStatement *let_statement_new(Token, Identifier *, Expression *);
String let_statement_token_literal(const Node *self);
String let_statement_string(const Node *self);
void let_statement_destroy(Node *self);

static const NodeVT LET_STATEMENT_VT = {
    ._t = STATEMENT,
    .token_literal = let_statement_token_literal,
    .string = let_statement_string,
    .destroy = let_statement_destroy,
};

typedef struct OperatorExpression {
  Expression base;
  Expression *left;
  Token op;
  Expression *right;
} OperatorExpr;

OperatorExpr *operator_expr_new(Expression *left, Token *op, Expression *right);
String operator_expr_token_literal(const Node *self);
String operator_expr_string(const Node *self);
void operator_expr_destroy(Node *self);

static const NodeVT OPERATOR_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = operator_expr_token_literal,
    .string = operator_expr_string,
    .destroy = operator_expr_destroy,
};

typedef struct IntExpr {
  Expression base;
  Token token;
  int32_t value;
} IntExpr;

IntExpr *int_expr_new(const Token t, int value);
String int_expr_token_literal(const Node *self);
String int_expr_string(const Node *self);
void int_expr_destroy(Node *self);

static const NodeVT INT_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = int_expr_token_literal,
    .string = int_expr_string,
    .destroy = int_expr_destroy,
};

typedef struct {
  Statement base;
  Token token;
  const Expression *value;
} ReturnStatement;

ReturnStatement *return_st_new(const Token t, const Expression *value);
String return_st_token_literal(const Node *self);
String return_st_string(const Node *self);
void return_st_destroy(Node *self);

static const NodeVT RETURN_ST_VT = {
    ._t = STATEMENT,
    .token_literal = return_st_token_literal,
    .string = return_st_string,
    .destroy = return_st_destroy,
};

typedef struct {
  Statement base;
  Token token;
  const Expression *expr;
} ExpressionStatement;

ExpressionStatement *expr_st_new(const Token t, const Expression *value);
String expr_st_token_literal(const Node *self);
String expr_st_string(const Node *self);
void expr_st_destroy(Node *self);

static const NodeVT EXPR_ST_VT = {
    ._t = STATEMENT,
    .token_literal = expr_st_token_literal,
    .string = expr_st_string,
    .destroy = expr_st_destroy,
};

typedef struct {
  Expression base;
  Token token;
  // Operator
  String op;
  /* Should be ideally an integer expression or ident which evaluates to integer
   * expr */
  Expression *right;
} PrefixExpression;

PrefixExpression *prefix_expr_new(const Token t, const String op,
                                  Expression *right);
String prefix_expr_token_literal(const Node *self);
String prefix_expr_string(const Node *self);
void prefix_expr_destroy(Node *self);

static const NodeVT PREFIX_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = prefix_expr_token_literal,
    .string = prefix_expr_string,
    .destroy = prefix_expr_destroy,
};

typedef struct {
  Expression base;
  Token token;
  Expression *left;
  String op;
  Expression *right;
} InfixExpression;

InfixExpression *infix_expr_new(const Token t, Expression *left,
                                const String op, Expression *right);
String infix_expr_token_literal(const Node *self);
String infix_expr_string(const Node *self);
void infix_expr_destroy(Node *self);

static const NodeVT INFIX_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = infix_expr_token_literal,
    .string = infix_expr_string,
    .destroy = infix_expr_destroy,
};

typedef struct {
  Expression base;
  Token token;
  bool value;
} BooleanExpression;

BooleanExpression *bool_expr_new(const Token t, bool value);
String bool_expr_token_literal(const Node *self);
String bool_expr_string(const Node *self);
void bool_expr_destroy(Node *self);

static const NodeVT BOOLEAN_EXPR_VT = {
    ._t = EXPRESSION,
    .token_literal = bool_expr_token_literal,
    .string = bool_expr_string,
    .destroy = bool_expr_destroy,
};

typedef struct {
  Statement base;
  Token token;
  StatementsArray statements;
} BlockStatement;

BlockStatement *block_statement_new(const Token t, StatementsArray);
String block_statement_token_literal(const Node *self);
String block_statement_string(const Node *self);
void block_statement_destroy(Node *self);

static const NodeVT BLOCK_STATEMENT_VT = {
    ._t = STATEMENT,
    .token_literal = block_statement_token_literal,
    .string = block_statement_string,
    .destroy = block_statement_destroy,
};

typedef struct {
  Expression base;
  Token token;
  Expression *condition;
  BlockStatement *consequence;
  BlockStatement *alternative;
} IfExpression;

IfExpression *if_expr_new(const Token t, Expression *condition,
                          BlockStatement *consequence,
                          BlockStatement *alternative);
String if_expr_token_literal(const Node *self);
String if_expr_string(const Node *self);
void if_expr_destroy(Node *self);

static const NodeVT IF_EXPRESSION_VT = {
    ._t = EXPRESSION,
    .token_literal = if_expr_token_literal,
    .string = if_expr_string,
    .destroy = if_expr_destroy,
};

typedef struct {
  Expression base;
  Token token;
  IdentifiersArray parameters;
  BlockStatement *body;
} FnExpression;

FnExpression *fn_expr_new(const Token t, IdentifiersArray parameters,
                          BlockStatement *body);
String fn_expr_token_literal(const Node *self);
String fn_expr_string(const Node *self);
void fn_expr_destroy(Node *self);

static const NodeVT FN_EXPRESSION_VT = {
    ._t = EXPRESSION,
    .token_literal = fn_expr_token_literal,
    .string = fn_expr_string,
    .destroy = fn_expr_destroy,
};

typedef struct {
  Expression base;
  Token token; // '(' token
  Expression *function;
  ExpressionsArray arguments;
} CallExpression;

CallExpression *call_expr_new(const Token t, Expression *function,
                              ExpressionsArray parameters);
String call_expr_token_literal(const Node *self);
String call_expr_string(const Node *self);
void call_expr_destroy(Node *self);

static const NodeVT CALL_EXPRESSION_VT = {
    ._t = EXPRESSION,
    .token_literal = call_expr_token_literal,
    .string = call_expr_string,
    .destroy = call_expr_destroy,
};

#endif // !AST_H
