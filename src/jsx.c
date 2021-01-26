#include "include/jsx.h"
#include "include/gen.h"
#include "include/string_utils.h"
#include "string.h"
#include <stdio.h>

AST_T* parse_template(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_TEMPLATE_STRING, parser->lexer->line);
  char* innerText = 0;
  token_T* tok = parser->token;
  innerText = strdup(tok->value);
  if (parser->token->type == TOKEN_ID) {
    tok = lexer_parse_any(parser->lexer, '<');
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
  ast->list_value = NEW_STACK;

  while (parser->lexer->c != '/' &&
         (parser->token->type == TOKEN_LT || parser->token->type == TOKEN_ID)) {
    if (parser->token->type == TOKEN_ID) {
      AST_T* template = parse_template(parser, options);
      list_push(ast->list_value, template);
    } else {
      AST_T* jsx = parse_jsx(parser, options);
      list_push(ast->list_value, jsx);
    }
  }

  return ast;
}

AST_T* parse_jsx_attr(parser_T* parser, parser_options_T options)
{
  AST_T* assignment = init_ast_line(AST_ASSIGNMENT, parser->lexer->line);
  AST_T* left = parser_parse_id(parser, options);

  gc_mark(parser->flags->GC, assignment);

  while (left && left->name && parser->token->type == TOKEN_MINUS) {
    left->name = str_append(&left->name, "-");
    parser_eat(parser, TOKEN_MINUS);
    char* next_str = parser->token->value;
    left->name = str_append(&left->name, next_str);
    parser_eat(parser, TOKEN_ID);
  }

  if (parser->token->type == TOKEN_EQUALS) {
    parser_eat(parser, TOKEN_EQUALS);
    assignment->value = parser_parse_factor(parser, options);

    if (assignment->value &&
        (assignment->value->type == AST_OBJECT || assignment->value->type == AST_SCOPE)) {
      char* generated = gen(assignment->value, parser->flags);

      if (generated) {
        char* value = strdup("$");
        value = str_append(&value, generated);
        AST_T* jsx_node = init_ast(AST_TEMPLATE_STRING);
        jsx_node->string_value = strdup(value);
        assignment->value = jsx_node;
        free(value);
      }
    }
  }

  assignment->left = left;

  if (left->name)
    assignment->name = strdup(left->name);

  return assignment;
}

AST_T* parse_jsx(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_JSX_ELEMENT, parser->lexer->line);
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

  parser_eat(parser, TOKEN_GT);

  ast->body = parse_jsx_compound(parser, options);

  parser_eat(parser, TOKEN_LT);
  parser_eat(parser, TOKEN_DIV);
  parser_eat(parser, TOKEN_ID);
  parser_eat(parser, TOKEN_GT);

  return ast;
}
