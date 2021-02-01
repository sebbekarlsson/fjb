#include "include/jsx.h"
#include "include/env.h"
#include "include/js.h"
#include "include/string_utils.h"
#include "string.h"
#include <stdio.h>

extern fjb_env_T* FJB_ENV;

AST_T* parse_template(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_TEMPLATE_STRING, parser->lexer->line);
  ast->parent = options.parent;
  char* innerText = 0;
  token_T* tok = parser->token;

  innerText = str_append(&innerText, tok->value);

  if (parser->token->type == TOKEN_ID) {
    tok = lexer_parse_any(parser->lexer, '<', '{');
    parser->token = tok;
    innerText = str_append(&innerText, tok->value);

    if (parser->token->type == TOKEN_RAW) {
      parser_eat(parser, TOKEN_RAW);
      ast->type = AST_JSX_TEXT;
    } else if (parser->token->type == TOKEN_TEMPLATE_STRING) {
      parser_eat(parser, TOKEN_TEMPLATE_STRING);
      ast->type = AST_JSX_TEMPLATE_STRING;
    }
  }

  ast->string_value = innerText;

  return ast;
}

AST_T* parse_jsx_compound(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_JSX_COMPOUND, parser->lexer->line);
  ast->parent = options.parent;
  ast->list_value = NEW_STACK;

  while (parser->lexer->c != '/' &&
         (parser->token->type == TOKEN_LT || parser->token->type == TOKEN_ID ||
          parser->token->type == TOKEN_LBRACE)) {
    if (parser->token->type == TOKEN_ID) {
      AST_T* template = parse_template(parser, options);
      list_push(ast->list_value, template);
    } else if (parser->token->type == TOKEN_LBRACE) {
      AST_T* node = parse_jsx_compute_block(parser, options);
      list_push(ast->list_value, node);
    } else {
      AST_T* jsx = parse_jsx(parser, options);
      if (!jsx->parent)
        jsx->parent = options.parent;
      list_push(ast->list_value, jsx);
    }
  }

  return ast;
}

AST_T* parse_jsx_attr_value(parser_T* parser, parser_options_T options)
{

  if (parser->token->type == TOKEN_LBRACE) {
    AST_T* ast = init_ast(AST_JSX_TEMPLATE_VALUE);
    ast->parent = options.parent;
    parser_eat(parser, TOKEN_LBRACE);
    ast->expr = parser_parse_expr(parser, options);
    parser_eat(parser, TOKEN_RBRACE);

    return ast;
  }

  return parser_parse_factor(parser, options);
}

AST_T* parse_jsx_compute_block(parser_T* parser, parser_options_T options)
{

  if (parser->token->type == TOKEN_LBRACE) {
    AST_T* ast = init_ast(AST_JSX_TEMPLATE_VALUE);
    ast->parent = options.parent;
    parser_eat(parser, TOKEN_LBRACE);
    ast->expr = parser_parse_expr(parser, options);
    parser_eat(parser, TOKEN_RBRACE);

    if (ast->expr) {
      if (ast->expr->type == AST_NAME) {
        ast->type = AST_JSX_TEMPLATE_STRING;
      }
    }

    return ast;
  }

  return parser_parse_factor(parser, options);
}

AST_T* parse_jsx_attr(parser_T* parser, parser_options_T options)
{
  AST_T* assignment = init_ast_line(AST_ASSIGNMENT, parser->lexer->line);
  assignment->parent = options.parent;
  AST_T* left = parser_parse_id(parser, options);

  gc_mark(parser->env->GC, assignment);

  while (left && left->name && parser->token->type == TOKEN_MINUS) {
    left->name = str_append(&left->name, "-");
    parser_eat(parser, TOKEN_MINUS);
    char* next_str = parser->token->value;
    left->name = str_append(&left->name, next_str);
    parser_eat(parser, TOKEN_ID);
  }

  if (left->name) {
    char* after = str_get_after(left->name, "on");
    if (after && is_js_event(after)) {
      left->name = strdup(after);
    }
  }

  if (parser->token->type == TOKEN_EQUALS) {
    parser_eat(parser, TOKEN_EQUALS);
    assignment->value = parse_jsx_attr_value(parser, options);
  }

  assignment->left = left;

  if (left->name)
    assignment->name = strdup(left->name);

  return assignment;
}

AST_T* parse_jsx(parser_T* parser, parser_options_T options)
{
  FJB_ENV->is_using_jsx = 1;
  AST_T* ast = init_ast_line(AST_JSX_ELEMENT, parser->lexer->line);
  ast->parent = options.parent;
  ast->options = NEW_STACK;
  ast->list_value = NEW_STACK;
  char* innerText = 0;
  unsigned int innerText_is_template = 0;

  parser_eat(parser, TOKEN_LT);

  ast->name = strdup(parser->token->value);
  parser_eat(parser, TOKEN_ID);

  while (parser->token->type == TOKEN_ID) {
    AST_T* assignment = parse_jsx_attr(parser, options);
    list_push(ast->options, assignment);
  }

  if (parser->token->type == TOKEN_DIV) {
    parser_eat(parser, TOKEN_DIV);
    parser_eat(parser, TOKEN_GT);
    return ast;
  }

  parser_eat(parser, TOKEN_GT);

  ast->body = parse_jsx_compound(parser, options);

  parser_eat(parser, TOKEN_LT);
  parser_eat(parser, TOKEN_DIV);
  parser_eat(parser, TOKEN_ID);
  parser_eat(parser, TOKEN_GT);

  return ast;
}
