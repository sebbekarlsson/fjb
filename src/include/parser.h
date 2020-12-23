#ifndef FJB_PARSER_H
#define FJB_PARSER_H
#include "AST.h"
#include "lexer.h"

typedef struct FJB_PARSER_STRUCT {
  lexer_T* lexer;
  token_T* token;
  int tmp_ast_type;
  const char* filepath;
} parser_T;

parser_T* init_parser(lexer_T* lexer, const char* filepath);

AST_T* parser_parse(parser_T* parser);

AST_T* parser_parse_expr(parser_T* parser);

AST_T* parser_parse_compound(parser_T* parser);

AST_T* parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse_term(parser_T* parser);

AST_T* parser_parse_factor(parser_T* parser);
#endif
