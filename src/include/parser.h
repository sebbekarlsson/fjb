#ifndef FJB_PARSER_H
#define FJB_PARSER_H
#include "AST.h"
#include "env.h"
#include "lexer.h"
#include "list.h"

typedef struct FJB_PARSER_STRUCT
{
  lexer_T* lexer;
  token_T* token;
  fjb_env_T* env;
} parser_T;

typedef struct FJB_PARSER_OPTIONS_STRUCT
{
  int stop_token;
  AST_T* parent;
  unsigned int not_exported;
} parser_options_T;

#define EMPTY_PARSER_OPTIONS                                                                       \
  {                                                                                                \
    -1, 0                                                                                          \
  }

parser_T* init_parser(lexer_T* lexer, fjb_env_T* env);

AST_T* parser_parse(parser_T* parser, parser_options_T options);

AST_T* parser_parse_any(parser_T* parser, parser_options_T options);

AST_T* parser_parse_while(parser_T* parser, parser_options_T options);

AST_T* parser_parse_call(parser_T* parser, parser_options_T options);

AST_T* parser_parse_id(parser_T* parser, parser_options_T options);

AST_T* parser_parse_object(parser_T* parser, parser_options_T options);

AST_T* parser_parse_object_child(parser_T* parser, parser_options_T options);

AST_T* parser_parse_interface(parser_T* parser, parser_options_T options);

AST_T* parser_parse_destructor(parser_T* parser, parser_options_T options);

AST_T* parser_parse_case(parser_T* parser, parser_options_T options);

AST_T* parser_parse_expr(parser_T* parser, parser_options_T options);

AST_T* parser_parse_definition(parser_T* parser, parser_options_T options);

AST_T* parser_parse_assignment(parser_T* parser, parser_options_T options, AST_T* id);

AST_T* parser_parse_compound(parser_T* parser, parser_options_T options);

AST_T* parser_parse_statement(parser_T* parser, parser_options_T options);

AST_T* parser_parse_statement_or_expr(parser_T* parser, parser_options_T options);

AST_T* parser_parse_array(parser_T* parser, parser_options_T options);

void parser_eat(parser_T* parser, int token_type);

void parser_eat_any(parser_T* parser);

AST_T* parser_parse_term(parser_T* parser, parser_options_T options);

AST_T* parser_parse_factor(parser_T* parser, parser_options_T options);

void parser_free(parser_T* parser);
#endif
