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

typedef struct Program {
  StatementsArray statements;
} Program;

String token_literal(Program *self);
String program_string(Program *self);

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
void push_error(Parser *self, String message);
void register_prefix(Parser *self, TokenType tt, PrefixParseFn fn);
void register_infix(Parser *self, TokenType tt, InfixParseFn fn);

Program *parse_program(Parser *self);

void print_errors(Parser *self);

Program *Program_new(StatementsArray);
void free_program(Program *);

// Prefix Expressions
LetStatement *parse_let_statement(Parser *self);
Identifier *parse_identifier(Parser *self);
Statement *parse_if_statement(Parser *self);
IfExpression *parse_if_expression(Parser *self);
ReturnStatement *parse_return_statement(Parser *self);
Expression *parse_expression(Parser *self, Precedence prec);
OperatorExpr *parse_operator_expr(Parser *self);
IntExpr *parse_int_expr(Parser *self);
Expression *parse_grouped_expression(Parser *self);
ExpressionStatement *parse_expression_statement(Parser *self);
BooleanExpression *parse_boolean_expression(Parser *self);
BlockStatement *parse_block_statement(Parser *self);
FnExpression *parse_func_expression(Parser *self);
IdentifiersArray parse_func_parameters(Parser *self);
PrefixExpression *parse_prefix_expression(Parser *self);

// Infix Expressions
CallExpression *parse_call_expression(Parser *self, Expression *left);
ExpressionsArray parse_argument_list(Parser *self);
InfixExpression *parse_infix_expression(Parser *self, Expression *left);

#endif // !PARSER_H
