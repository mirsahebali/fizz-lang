#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "lexer.h"
#include "utils.h"

#include "cstring.h/cstring.h"

StatementsArray statements_array_init(int32_t capacity) {

  assert(capacity >= 0);
  StatementsArray s;

  s.size = 0;
  s.capacity = capacity == 0 ? 16 : capacity;
  s.data = malloc(sizeof(Statement *) * s.capacity);
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
    if (s->data[i] == NULL)
      continue;
    s->data[i]->vt->destroy(s->data[i]);
  }

  free(s->data);
  s->size = 0;
  s->capacity = 0;
}

// IdentifiersArray impl start -----
IdentifiersArray identifiers_array_init(int32_t capacity) {

  assert(capacity >= 0);
  IdentifiersArray arr;

  arr.size = 0;
  arr.capacity = capacity == 0 ? 16 : capacity;
  arr.data = malloc(sizeof(Identifier *) * arr.capacity);
  assert(arr.data != NULL);
  return arr;
}

bool identifiers_reserve(IdentifiersArray *self, int32_t new_capacity) {
  assert(self != NULL);
  if (new_capacity <= self->capacity)
    return true;
  Identifier **new_ptr =
      realloc(self->data, sizeof(Identifier *) * new_capacity);

  if (!new_ptr)
    return false;

  self->data = new_ptr;
  self->capacity = new_capacity;
  return true;
}
// realloc the area and push the data
int32_t identifiers_push(IdentifiersArray *self, Identifier *data) {
  assert(self != NULL);
  if (self->capacity == self->size) {
    int32_t new_cap = self->capacity > 0 ? self->capacity * 2 : 1;
    assert(identifiers_reserve(self, new_cap));
  }

  self->data[self->size] = data;
  return ++self->size;
}

Identifier *identifiers_get(IdentifiersArray *self, int32_t index) {
  assert(self != NULL);
  assert(index >= 0);
  assert(index < self->size);

  return self->data[index];
}

int32_t identifiers_size(IdentifiersArray *self) {
  assert(self != NULL);
  return self->size;
}
int32_t identifiers_capacity(IdentifiersArray *self) {
  assert(self != NULL);
  return self->capacity;
}
void free_identifiers(IdentifiersArray *self) {
  if (self == NULL)
    return;

  for (int i = 0; i < self->size; i++) {
    if (self->data[i] == NULL)
      continue;
    self->data[i]->base.vt->destroy((Node *)self->data[i]);
  }

  free(self->data);
  self->size = 0;
  self->capacity = 0;
}

// IdentifiersArray impl end -----

// ExpressionsArray impl start -----
ExpressionsArray expressions_array_init(int32_t capacity) {

  assert(capacity >= 0);
  ExpressionsArray arr;

  arr.size = 0;
  arr.capacity = capacity == 0 ? 16 : capacity;
  arr.data = malloc(sizeof(Expression *) * arr.capacity);
  assert(arr.data != NULL);
  return arr;
}

bool expressions_reserve(ExpressionsArray *self, int32_t new_capacity) {
  assert(self != NULL);
  if (new_capacity <= self->capacity)
    return true;
  Expression **new_ptr =
      realloc(self->data, sizeof(Expression *) * new_capacity);

  if (!new_ptr)
    return false;

  self->data = new_ptr;
  self->capacity = new_capacity;
  return true;
}
// realloc the area and push the data
int32_t expressions_push(ExpressionsArray *self, Expression *data) {
  assert(self != NULL);
  if (self->capacity == self->size) {
    int32_t new_cap = self->capacity > 0 ? self->capacity * 2 : 1;
    assert(expressions_reserve(self, new_cap));
  }

  self->data[self->size] = data;
  return ++self->size;
}

Expression *expressions_get(ExpressionsArray *self, int32_t index) {
  assert(self != NULL);
  assert(index >= 0);
  assert(index < self->size);

  return self->data[index];
}

int32_t expressions_size(ExpressionsArray *self) {
  assert(self != NULL);

  return self->size;
}
int32_t expressions_capacity(ExpressionsArray *self) {
  assert(self != NULL);
  return self->capacity;
}

void free_expressions(ExpressionsArray *self) {
  if (self == NULL)
    return;

  for (int i = 0; i < self->size; i++) {
    if (self->data[i] == NULL)
      continue;
    self->data[i]->vt->destroy(self->data[i]);
  }

  free(self->data);
  self->size = 0;
  self->capacity = 0;
}
// ExpressionsArray impl end -----

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

  return String_clone(&ident->value);
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

#ifdef DEBUG
  printf("Identifier destroyed\n");
#endif
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

  String value = let_st->value->vt->string((Node *)let_st->value);
  String name_str = let_st->name->base.vt->token_literal((Node *)let_st->name);
  String eq_str = STR_NEW(" = ");
  String spc_str = STR_NEW(" ");
  String semicolon = STR_NEW(";");
  String out = String_join(6, &let_st->token.literal, &spc_str, &name_str,
                           &eq_str, &value, &semicolon);

  free_string(&value);
  free_string(&name_str);

  return out;
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

#ifdef DEBUG
  printf("LetStatement destroyed\n");
#endif

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

#ifdef DEBUG
  printf("OprExpr destroyed \n");
#endif
}

IntExpr *int_expr_new(const Token t, const int value) {
  IntExpr *int_expr = malloc(sizeof(IntExpr));
  assert(int_expr != NULL);

  int_expr->base.vt = &INT_EXPR_VT;
  int_expr->token = t;
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
  if (self == NULL)
    return;
  IntExpr *int_expr = (IntExpr *)self;
  free_token(&int_expr->token);

#ifdef DEBUG
  printf("IntExpr Destroyed\n");
#endif
  free(self);
}

ReturnStatement *return_st_new(const Token t, const Expression *value) {
  ReturnStatement *ret_st = malloc(sizeof(ReturnStatement));

  assert(ret_st != NULL);

  ret_st->base.vt = &RETURN_ST_VT;
  ret_st->token = t;
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

  String return_st_token = ret_st->base.vt->token_literal((Node *)ret_st);
  String spc = STR_NEW(" ");
  String ret_val = ret_st->value->vt->string(ret_st->value);
  String semi_colon = STR_NEW(";");

  String out = String_join(4, &return_st_token, &spc, &ret_val, &semi_colon);
  free_string(&return_st_token);
  free_string(&ret_val);

  return out;
}
void return_st_destroy(Node *self) {
  if (self == NULL) {
    return;
  }
  ReturnStatement *ret_st = (ReturnStatement *)self;
  assert(ret_st != NULL);

  if (ret_st->value != NULL) {
    ret_st->value->vt->destroy((Node *)ret_st->value);
  }
  free_token(&ret_st->token);
  free(self);

#ifdef DEBUG
  printf("Return Statement destroyed\n");
#endif
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
  ExpressionStatement *expr_st = (ExpressionStatement *)self;

  assert(expr_st->expr != NULL);

  String expr_string = expr_st->expr->vt->string((Node *)expr_st->expr);

  return expr_string;
}

void expr_st_destroy(Node *self) {
  ExpressionStatement *st_expr = (ExpressionStatement *)self;
  assert(st_expr != NULL);

  free_token(&st_expr->token);
  if (st_expr->expr != NULL)
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
  String right_expr_str =
      prefix_expr->right->vt->string((Node *)prefix_expr->right);
  String left_paren = STR_NEW("(");
  String right_paren = STR_NEW(")");
  String out = String_join(4, &left_paren, &prefix_expr->op, &right_expr_str,
                           &right_paren);

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
  String spc = STR_NEW(" ");
  String left_str = infix_expr->left->vt->string((Node *)infix_expr->left);
  String right_str = infix_expr->right->vt->string((Node *)infix_expr->right);
  String out_str = String_join(7, &l_paren, &left_str, &spc, &infix_expr->op,
                               &spc, &right_str, &r_paren);

  free_string(&left_str);
  free_string(&right_str);
  return out_str;
}
void infix_expr_destroy(Node *self) {
  if (self == NULL)
    return;
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

BooleanExpression *bool_expr_new(const Token t, bool value) {
  BooleanExpression *bool_expr = malloc(sizeof(BooleanExpression));
  bool_expr->base.vt = &BOOLEAN_EXPR_VT;
  bool_expr->token = t;
  bool_expr->value = value;
  return bool_expr;
}
String bool_expr_token_literal(const Node *self) {
  assert(self != NULL);
  BooleanExpression *bool_expr = (BooleanExpression *)self;
  return String_clone(&bool_expr->token.literal);
}
String bool_expr_string(const Node *self) {
  assert(self != NULL);
  BooleanExpression *bool_expr = (BooleanExpression *)self;
  return String_clone(&bool_expr->token.literal);
}
void bool_expr_destroy(Node *self) {
  if (self == NULL)
    return;

  BooleanExpression *bool_expr = (BooleanExpression *)self;
  free_token(&bool_expr->token);

  free(self);
}

BlockStatement *block_statement_new(const Token t, StatementsArray statements) {
  BlockStatement *block_st = (BlockStatement *)malloc(sizeof(BlockStatement));
  block_st->base.vt = &BLOCK_STATEMENT_VT;
  block_st->token = t;
  block_st->statements = statements;
  return block_st;
}
String block_statement_token_literal(const Node *self) {
  assert(self != NULL);
  BlockStatement *block_st = (BlockStatement *)self;
  return String_clone(&block_st->token.literal);
}
String block_statement_string(const Node *self) {
  assert(self != NULL);
  BlockStatement *block_st = (BlockStatement *)self;
  StringArray string_array = string_array_init(block_st->statements.size + 1);
  for (int i = 0; i < block_st->statements.size; i++) {
    Statement *st = statements_get(&block_st->statements, i);
    string_array_push(&string_array, st->vt->string(st));
  }
  String out = string_array_join(&string_array, STR_NEW(""));
  free_string_array(&string_array);
  return out;
}
void block_statement_destroy(Node *self) {
  if (self == NULL)
    return;
  BlockStatement *block_st = (BlockStatement *)self;
  free_statements(&block_st->statements);
  free_token(&block_st->token);
  free(self);
}

IfExpression *if_expr_new(const Token t, Expression *condition,
                          BlockStatement *consequence,
                          BlockStatement *alternative) {
  IfExpression *if_expr = (IfExpression *)malloc(sizeof(IfExpression));
  if_expr->base.vt = &IF_EXPRESSION_VT;
  if_expr->token = t;
  if_expr->condition = condition;
  if_expr->alternative = alternative;
  if_expr->consequence = consequence;
  return if_expr;
}
String if_expr_token_literal(const Node *self) {
  assert(self != NULL);
  IfExpression *if_expr = (IfExpression *)self;

  return String_clone(&if_expr->token.literal);
}
String if_expr_string(const Node *self) {
  assert(self != NULL);
  IfExpression *if_expr = (IfExpression *)self;

  String if_str = STR_NEW("if");
  String empty_str = STR_NEW(" ");
  String l_brace = STR_NEW("{");
  String r_brace = STR_NEW("}");
  String cond_str = if_expr->condition->vt->string(if_expr->condition);
  String cons_str =
      if_expr->consequence->base.vt->string((Node *)if_expr->consequence);

  String out = String_join(6, &if_str, &cond_str, &empty_str, &l_brace,
                           &cons_str, &r_brace);

  if (if_expr->alternative != NULL) {

    String alt_str =
        if_expr->alternative->base.vt->string((Node *)if_expr->alternative);
    String else_str = STR_NEW("else ");

    String temp = String_clone(&out);
    free_string(&out);

    out = String_join(7, &temp, &else_str, &l_brace, &spc, &alt_str, &spc,
                      &r_brace);
    free_string(&alt_str);
  }

  free_string(&cond_str);
  free_string(&cons_str);

  return out;
}
void if_expr_destroy(Node *self) {
  if (self == NULL)
    return;

  IfExpression *if_expr = (IfExpression *)self;
  free_token(&if_expr->token);
  if (if_expr->condition != NULL)
    if_expr->condition->vt->destroy(if_expr->condition);

  if (if_expr->consequence != NULL)
    if_expr->consequence->base.vt->destroy((Node *)if_expr->consequence);

  if (if_expr->alternative != NULL)
    if_expr->alternative->base.vt->destroy((Node *)if_expr->alternative);
  free(self);
}

FnExpression *fn_expr_new(const Token t, IdentifiersArray parameters,
                          BlockStatement *body) {
  FnExpression *fn_expr = (FnExpression *)malloc(sizeof(FnExpression));
  fn_expr->base.vt = &FN_EXPRESSION_VT;
  fn_expr->token = t;
  fn_expr->parameters = parameters;
  fn_expr->body = body;
  return fn_expr;
}
String fn_expr_string(const Node *self) {
  assert(self != NULL);
  FnExpression *fn_expr = (FnExpression *)self;
  String fn_str = STR_NEW("fn");
  String l_paren = STR_NEW("(");
  String r_paren = STR_NEW(")");
  String l_brace = STR_NEW("{");
  String r_brace = STR_NEW("}");
  String spc = STR_NEW(" ");
  String comma = STR_NEW(", ");
  // getting the identifiers out
  StringArray params_arr = string_array_init(fn_expr->parameters.size);
  for (int i = 0; i < fn_expr->parameters.size; i++) {
    Identifier *ident = identifiers_get(&fn_expr->parameters, i);
    String val = String_clone(&ident->value);
    string_array_push(&params_arr, val);
  }

  String params_str = string_array_join(&params_arr, comma);
  String block_st_str = fn_expr->body->base.vt->string((Node *)fn_expr->body);

  String out = String_join(9, &fn_str, &l_paren, &params_str, &r_paren,
                           &l_brace, &spc, &block_st_str, &spc, &r_brace);

  free_string(&params_str);
  free_string(&block_st_str);
  free_string_array(&params_arr);
  return out;
}
String fn_expr_token_literal(const Node *self) {
  assert(self != NULL);

  FnExpression *fn_expr = (FnExpression *)self;
  return String_clone(&fn_expr->token.literal);
}
void fn_expr_destroy(Node *self) {
  if (self == NULL)
    return;

  FnExpression *fn_expr = (FnExpression *)self;
  free_token(&fn_expr->token);
  free_identifiers(&fn_expr->parameters);
  fn_expr->body->base.vt->destroy((Node *)fn_expr->body);

  free(self);
}

CallExpression *call_expr_new(const Token t, Expression *function,
                              ExpressionsArray arguments) {
  CallExpression *call_expr = (CallExpression *)malloc(sizeof(CallExpression));

  call_expr->base.vt = &CALL_EXPRESSION_VT;
  call_expr->token = t;
  call_expr->function = function;
  call_expr->arguments = arguments;

  return call_expr;
}
String call_expr_token_literal(const Node *self) {
  assert(self != NULL);

  CallExpression *call_expr = (CallExpression *)self;
  return String_clone(&call_expr->token.literal);
}
String call_expr_string(const Node *self) {
  assert(self != NULL);

  CallExpression *call_expr = (CallExpression *)self;
  String l_paren = STR_NEW("(");
  String r_paren = STR_NEW(")");
  String ident_str = call_expr->function->vt->string(call_expr->function);
  StringArray argument_list = string_array_init(call_expr->arguments.size);
  for (int32_t i = 0; i < call_expr->arguments.size; i++) {
    Expression *arg = expressions_get(&call_expr->arguments, i);
    string_array_push(&argument_list, arg->vt->string(arg));
  }

  String expr_list_str = string_array_join(&argument_list, STR_NEW(", "));

  String out = String_join(4, &ident_str, &l_paren, &expr_list_str, &r_paren);
  free_string(&expr_list_str);
  free_string(&ident_str);
  free_string_array(&argument_list);
  return out;
}
void call_expr_destroy(Node *self) {
  if (self == NULL)
    return;

  CallExpression *call_expr = (CallExpression *)self;
  call_expr->function->vt->destroy(call_expr->function);
  free_token(&call_expr->token);
  free_expressions(&call_expr->arguments);

  free(self);
}
