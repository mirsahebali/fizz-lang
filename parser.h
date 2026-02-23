#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

#include "lexer.h"

#include "ast.h"

typedef enum Precedence {
  PREC_INVALID,
  PREC_LOWEST,
  PREC_EQUALS,      // ==
  PREC_LESSGREATER, // > or <
  PREC_SUM,         // +
  PREC_PRODUCT,     // *
  PREC_PREFIX,      // -X or !X
  PREC_FN_CALL,     // someFunction(X)
} Precedence;

Precedence precedence_map(TokenType);

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

typedef struct Program {
  StatementsArray statements;
} Program;

String token_literal(Program *self);
String *program_string(Program *self);

typedef struct Parser Parser;

typedef Expression *(*PrefixParseFn)(Parser *self);
typedef Expression *(*InfixParseFn)(Parser *self, Expression *expr);

struct Parser {
  Lexer *lexer;
  Token curr_token;
  Token peek_token;
  StringArray errors;

  PrefixParseFn prefix_parse_fns[TOKEN_COUNT];
  InfixParseFn infix_parse_fns[TOKEN_COUNT];
};

Parser *Parser_new(Lexer *);
void free_parser(Parser *);

void parser_next_token(Parser *self);
const StringArray *parser_errors(const Parser *self);
void peek_error(Parser *self, const TokenType tt);
void register_prefix(Parser *self, TokenType tt, PrefixParseFn fn);
void register_infix(Parser *self, TokenType tt, InfixParseFn fn);

Program *parse_program(Parser *self);

void print_errors(Parser *self);

Program *Program_new(StatementsArray);
void free_program(Program *);

LetStatement *parse_let_statement(Parser *self);
Identifier *parse_identifier(Parser *self);
Statement *parse_if_statement(Parser *self);
ReturnStatement *parse_return_statement(Parser *self);
Expression *parse_expression(Parser *self, Precedence prec);
OperatorExpr *parse_operator_expr(Parser *self);
IntExpr *parse_int_expr(Parser *self);
Expression *parse_grouped_expr(Parser *self);
ExpressionStatement *parse_expression_statement(Parser *self);

PrefixExpression *parse_prefix_expression(Parser *self);
InfixExpression *parse_infix_expression(Parser *self, Expression *left);

#endif // !PARSER_H
