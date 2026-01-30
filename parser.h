#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

#include "lexer.h"

#include "ast.h"

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

String token_literal(Program *);

typedef struct Parser {
  Lexer *lexer;
  Token curr_token;
  Token peek_token;
  StringArray errors;
} Parser;

Parser *Parser_new(Lexer *);
void free_parser(Parser *);

void parser_next_token(Parser *self);
const StringArray *parser_errors(const Parser *self);
void peek_error(Parser *self, const TokenType tt);

Program *parse_program(Parser *self);

void print_errors(Parser *self);

Program *Program_new(StatementsArray);
void free_program(Program *);

LetStatement *parse_let_statement(Parser *self);
Identifier *parse_identifier(Parser *self);
Statement *parse_if_statement(Parser *self);
Statement *parse_return_statement(Parser *self);
Expression *parse_expression(Parser *self);
OperatorExpr *parse_operator_expr(Parser *self);
Expression *parse_integer_literal(Parser *self);
Expression *parse_grouped_expr(Parser *self);

#endif // !PARSER_H
