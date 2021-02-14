#include "include/parse_typescript.h"
#include <stdio.h>
#include <string.h>

AST_T* parser_parse_interface_child(parser_T* parser, parser_options_T options)
{
  AST_T* colon_ass = init_ast_line(AST_COLON_ASSIGNMENT, parser->lexer->line);
  colon_ass->parent = options.parent;

  colon_ass->left = parser_parse_factor(parser, options);

  if (parser->token->type != TOKEN_COLON) {
    return colon_ass->left;
  }

  parser_eat(parser, TOKEN_COLON);

  colon_ass->right = parser_parse_typehints(parser, options);

  if (colon_ass->left) {
    char* name = ast_get_string(colon_ass->left);

    if (name)
      colon_ass->name = strdup(name);
  }

  gc_mark(parser->env->GC, colon_ass);
}

AST_T* parser_parse_interface(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_INTERFACE, parser->lexer->line);
  ast->list_value = NEW_STACK;
  ast->options = NEW_STACK;
  EAT(TOKEN_INTERFACE);

  if (parser->token->type == TOKEN_ID) {
    ast->name = strdup(parser->token->value);
    EAT(TOKEN_ID);
  }

  // extends ... etc
  while (parser->token->type != TOKEN_LBRACE) {
    AST_T* statement = parser_parse_statement(parser, options);
    list_push(ast->options, statement);
  }

  EAT(TOKEN_LBRACE);

  if (parser->token->type != TOKEN_RBRACE) {
    AST_T* child = parser_parse_interface_child(parser, options);
    list_push(ast->list_value, child);

    while ((parser->token->type == TOKEN_SEMI || parser->token->type == TOKEN_COMMA) &&
           parser->token->type != TOKEN_RBRACE) {
      if (parser->token->type == TOKEN_SEMI)
        EAT(TOKEN_SEMI);
      else if (parser->token->type == TOKEN_COMMA)
        EAT(TOKEN_COMMA);

      AST_T* child = parser_parse_interface_child(parser, options);
      list_push(ast->list_value, child);
    }
  }

  EAT(TOKEN_RBRACE);

  return ast;
}

AST_T* parser_parse_type(parser_T* parser, parser_options_T options)
{
  AST_T* name = init_ast_line(AST_DATA_TYPE, parser->lexer->line);
  name->name = strdup(parser->token->value);
  parser_eat_any(parser);

  return name;
}

AST_T* parser_parse_custom_type(parser_T* parser, parser_options_T options)
{
  AST_T* name = init_ast_line(AST_DATA_TYPE, parser->lexer->line);
  name->name = strdup(parser->token->value);
  parser_eat_any(parser);

  return name;
}

AST_T* parser_parse_hint(parser_T* parser, parser_options_T options)
{
  AST_T* left = 0;

  switch (parser->token->type) {
    case TOKEN_TYPE_NUMBER:
    case TOKEN_TYPE_STRING: left = parser_parse_type(parser, options); break;
    case TOKEN_ID: left = parser_parse_custom_type(parser, options); break;
    default: {
      left = 0;
    }
  }

  return left;
}

AST_T* parser_parse_typehints(parser_T* parser, parser_options_T options)
{
  AST_T* hint = 0;
  AST_T* left = 0;

  if (parser->token->type == TOKEN_COLON) {
    EAT(TOKEN_COLON);
  }

  left = parser_parse_hint(parser, options);

  while (left && (parser->token->type == TOKEN_PIPE || parser->token->type == TOKEN_PIPE_PIPE ||
                  parser->token->type == TOKEN_AND || parser->token->type == TOKEN_AND_AND)) {
    AST_T* binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_typehints(parser, options);
    left = binop;
  }

  return left;
}
