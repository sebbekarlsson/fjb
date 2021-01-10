#include "include/parser.h"
#include "include/array_utils.h"
#include "include/fjb.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/node.h"
#include "include/gc.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern gc_T* GC;
extern AST_T* NOOP;

parser_T* init_parser(lexer_T* lexer, const char* filepath)
{
  parser_T* parser = calloc(1, sizeof(struct FJB_PARSER_STRUCT));
  parser->lexer = lexer;
  parser->token = lexer_next(lexer);
  parser->filepath = filepath;

  return parser;
}

static void print_debug(parser_T* parser)
{
  printf("DEBUG: %s:%d:%s\n", parser->filepath, parser->lexer->line, parser->token->value);
}

static list_T* parse_args(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_LPAREN);
  list_T* list_value = init_list(sizeof(AST_T*));

  if (parser->token->type != TOKEN_RPAREN) {
    AST_T* child = parser_parse_expr(parser, options);
    list_push(list_value, child);

    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      child = parser_parse_expr(parser, options);
      list_push(list_value, child);
    }
  }

  parser_eat(parser, TOKEN_RPAREN);

  return list_value;
}

static list_T* parse_tuple(parser_T* parser, parser_options_T options)
{
  list_T* list_value = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA);
    AST_T* child = parser_parse_expr(parser, options);

    if (child->type != AST_NOOP) {
      list_push(list_value, child);
    }
  }

  return list_value;
}

static list_T* parse_tuple_term(parser_T* parser, parser_options_T options)
{
  list_T* list_value = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA);
    AST_T* child = parser_parse_term(parser, options);

    if (child->type != AST_NOOP) {
      list_push(list_value, child);
    }
  }

  return list_value;
}

static list_T* parse_semi_args(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_LPAREN);
  list_T* list_value = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_SEMI) {
    parser_eat(parser, TOKEN_SEMI);
    list_push(list_value, NOOP);
  }

  if (parser->token->type != TOKEN_RPAREN) {
    AST_T* child = parser_parse_expr(parser, options);
    list_push(list_value, child);

    while (parser->token->type != TOKEN_RPAREN) {
      if (parser->token->type == TOKEN_SEMI)
        parser_eat(parser, TOKEN_SEMI);

      child = parser_parse_expr(parser, options);
      list_push(list_value, child);
    }
  }

  parser_eat(parser, TOKEN_RPAREN);

  return list_value;
}

static list_T* parse_array(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_LBRACKET);
  list_T* list_value = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA);
    AST_T* undef = init_ast_line(AST_UNDEFINED, parser->lexer->line);
    gc_mark(GC, undef);
    list_push(list_value, undef);
  }

  if (parser->token->type != TOKEN_RBRACKET) {
    AST_T* child = parser_parse_statement_or_expr(parser, options);
    list_push(list_value, child);

    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      child = parser_parse_statement_or_expr(parser, options);
      list_push(list_value, child);
    }
  }

  parser_eat(parser, TOKEN_RBRACKET);

  return list_value;
}

static AST_T* parse_block_linked_list(parser_T* parser, int types[], unsigned int length, parser_options_T options)
{
  AST_T* left = 0;

  for (unsigned int i = 0; i < length; i++) {
    if (parser->token->type == types[i]) {
      left = init_ast_line(AST_TRY, parser->lexer->line);
      left->token = token_clone(parser->token);
      parser_eat(parser, parser->token->type);

      if (parser->token->type == TOKEN_LPAREN) {
        left->list_value = parse_args(parser, options);
      }

      if (parser->token->type == TOKEN_LBRACE) {
        parser_eat(parser, TOKEN_LBRACE);
        left->body = parser_parse(parser, options);
        parser_eat(parser, TOKEN_RBRACE);
      }

      if (left) {
        left->right = parse_block_linked_list(parser, types, length, options);
        gc_mark(GC, left);
      }
    }
  }

  return left;
}

AST_T* parser_parse(parser_T* parser, parser_options_T options)
{
  return parser_parse_compound(parser, options);
}

AST_T* parser_parse_id(parser_T* parser, parser_options_T options, unsigned int with_assignment)
{
  AST_T* ast = init_ast_line(AST_NAME, parser->lexer->line);
  ast->is_ref = 1;
  ast->string_value = strdup(parser->token->value);
  ast->name = strdup(ast->string_value);

  gc_mark(GC, ast);

  parser_eat_any(parser);

  if (parser->token->type == TOKEN_DECREMENT || parser->token->type == TOKEN_INCREMENT) {
    AST_T* unop = init_ast_line(AST_UNOP, parser->lexer->line);
    unop->left = ast;
    unop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    gc_mark(GC, unop);
    return unop;
  }

  while (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    if (ast && ast->name)
    {
      ast_call->name = strdup(ast->name);
    }
    ast_call->left = ast;
    ast = ast_call;
    return ast;
  }

  if (with_assignment)
  {
    if (parser->token->type == TOKEN_EQUALS)
    {
      AST_T* astname = parser_parse_assignment(parser, options);
      astname->left = ast;
      astname->name = strdup(ast->name);
    
      ast = astname;
    }
  }

  return ast;
}

AST_T* parser_parse_int(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_INT, parser->lexer->line);
  ast->int_value = atoi(parser->token->value);
  parser_eat(parser, TOKEN_INT);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_float(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_FLOAT, parser->lexer->line);
  ast->float_value = atof(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_FLOAT);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_string(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_STRING, parser->lexer->line);

  ast->string_value = calloc(strlen(parser->token->value) + 1, sizeof(char));
  strcpy(ast->string_value, parser->token->value);
  parser_eat(parser, TOKEN_STRING);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_hex(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_HEX, parser->lexer->line);

  ast->string_value = calloc(strlen(parser->token->value) + 1, sizeof(char));
  strcpy(ast->string_value, parser->token->value);
  parser_eat(parser, TOKEN_HEX);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_import(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_IMPORT, parser->lexer->line);
  parser_eat(parser, TOKEN_IMPORT);

  ast->list_value = init_list(sizeof(AST_T*));

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);

    AST_T* ast_import_arg = parser_parse_id(parser, options, 0);
    list_push(ast->list_value, ast_import_arg);


    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      AST_T* ast_import_arg = parser_parse_id(parser, options, 0);
      list_push(ast->list_value, ast_import_arg);
    }

    parser_eat(parser, TOKEN_RBRACE);
  } else if (parser->token->type != TOKEN_STRING) {
    AST_T* ast_import_arg = parser_parse_id(parser, options, 0);
    list_push(ast->list_value, ast_import_arg);
  }

  if (ast->list_value->size > 0) {
    parser_eat(parser, TOKEN_FROM);
  }
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_STRING);

  gc_mark(GC, ast); 

  return ast;
}

AST_T* parser_parse_definition(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_DEFINITION, parser->lexer->line);
  ast->flags = init_list(sizeof(AST_T*));

  while (
    parser->token->type == TOKEN_CONST ||
    parser->token->type == TOKEN_LET ||
    parser->token->type == TOKEN_VAR) {
    AST_T* ast_flag = init_ast_line(AST_NAME, parser->lexer->line);
    ast_flag->string_value = strdup(parser->token->value);
    gc_mark(GC, ast_flag);
    list_push(ast->flags, ast_flag);
    parser_eat(parser, parser->token->type);
  }

  AST_T* left = parser_parse_id(parser, options, 1);
  left->def = ast;
  ast->expr = left;

  ast->list_value = init_list(sizeof(AST_T*));
  list_push(ast->list_value, left);

  while (parser->token->type == TOKEN_COMMA)
  {
    parser_eat(parser, TOKEN_COMMA);
    AST_T* id = parser_parse_id(parser, options, 1);
    id->def = ast;
    list_push(ast->list_value, id);
  }

  return ast;
}

AST_T* parser_parse_assignment(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_ASSIGNMENT, parser->lexer->line);
  
  while (parser->token->type == TOKEN_EQUALS) {
    parser_eat(parser, TOKEN_EQUALS);
    ast->value = parser_parse_term(parser, options);

    if (parser->token->type == TOKEN_EQUALS)
    {
      AST_T* binop = init_ast_line(AST_BINOP, parser->lexer->line);
      binop->left = ast->value;
      binop->right = parser_parse_term(parser, options);

      ast->value = binop;
    }
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_state(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_STATE, parser->lexer->line);
  ast->string_value = strdup(parser->token->value);
  ast->name = strdup(ast->string_value ? ast->string_value : ast->name);

  parser_eat(parser, parser->token->type);

  if (parser->token->type != TOKEN_SEMI)
    ast->value = parser_parse_term(parser, options);

  if (strcmp(ast->name, "export") == 0 && ast->value)
  {
    ast->value->exported = 1;
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_try(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_TRY, parser->lexer->line);
  ast->token = token_clone(parser->token);
  parser_eat(parser, TOKEN_TRY);

  parser_eat(parser, TOKEN_LBRACE);
  ast->body = parser_parse(parser, options);
  parser_eat(parser, TOKEN_RBRACE);

  ast->right = parse_block_linked_list(parser, (int[]){ TOKEN_CATCH, TOKEN_FINALLY }, 2, options);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_condition(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_CONDITION, parser->lexer->line);
  ast->token = token_clone(parser->token);

  if (parser->token->type == TOKEN_ELSE) {
    parser_eat(parser, TOKEN_ELSE);
  }

  if (parser->token->type == TOKEN_IF) {
    parser_eat(parser, TOKEN_IF);

    parser_eat(parser, TOKEN_LPAREN);
    ast->expr = parser_parse_expr(parser, options);
    parser_eat(parser, TOKEN_RPAREN);
  }

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = parser_parse(parser, options);
    }

    parser_eat(parser, TOKEN_RBRACE);
  } else if (parser->token->type != TOKEN_SEMI) {
    ast->body = parser_parse_statement_or_expr(parser, options);
  }

  if (parser->token->type == TOKEN_SEMI) {
    parser_eat(parser, TOKEN_SEMI);
  }

  if (parser->token->type == TOKEN_ELSE && ast->token->type != TOKEN_ELSE) {
    ast->right = parser_parse_condition(parser, options);
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_do(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_DO, parser->lexer->line);
  parser_eat(parser, TOKEN_DO);

  parser_eat(parser, TOKEN_LBRACE);

  if (parser->token->type != TOKEN_RBRACE) {
    ast->body = parser_parse(parser, options);
  }

  parser_eat(parser, TOKEN_RBRACE);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_while(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_WHILE);
  AST_T* ast = init_ast_line(AST_WHILE, parser->lexer->line);

  parser_eat(parser, TOKEN_LPAREN);
  ast->expr = parser_parse_expr(parser, options);
  parser_eat(parser, TOKEN_RPAREN);

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = parser_parse(parser, options);
    }

    parser_eat(parser, TOKEN_RBRACE);
  } else if (parser->token->type != TOKEN_SEMI) {
    ast->body = parser_parse_statement_or_expr(parser, options);
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_for(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_FOR);
  AST_T* ast = init_ast_line(AST_FOR, parser->lexer->line);
  ast->list_value = parse_semi_args(parser, options);

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);
    ast->body = parser_parse(parser, options);
    parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->body = parser_parse_statement_or_expr(parser, options);
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_switch(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_SWITCH);
  AST_T* ast = init_ast_line(AST_SWITCH, parser->lexer->line);
  parser_eat(parser, TOKEN_LPAREN);
  ast->expr = parser_parse_expr(parser, options);
  parser_eat(parser, TOKEN_RPAREN);

  parser_eat(parser, TOKEN_LBRACE);
  ast->body = parser_parse(parser, options);
  parser_eat(parser, TOKEN_RBRACE);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_array(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_ARRAY, parser->lexer->line);
  ast->list_value = parse_array(parser, options);
  
  gc_mark(GC, ast);

  while (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    if (ast && ast->name)
    {
      ast_call->name = strdup(ast->name);
    }
    ast_call->left = ast;
    ast = ast_call;
  }


  return ast;
}

AST_T* parser_parse_regex(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_REGEX, parser->lexer->line);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_REGEX);

  gc_mark(GC, ast);
  return ast;
}

AST_T* parser_parse_object(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_OBJECT, parser->lexer->line);
  ast->list_value = init_list(sizeof(token_T*));

  unsigned int is_object = 0;
  parser_eat(parser, TOKEN_LBRACE);

  if (parser->token->type != TOKEN_RBRACE) {
    list_push(ast->list_value, parser_parse_statement_or_expr(parser, options));

    while (parser->token->type != TOKEN_RBRACE) {
      if (parser->token->type == TOKEN_COMMA)
        parser_eat(parser, TOKEN_COMMA);

      if (parser->token->type != TOKEN_RBRACE) {
        list_push(ast->list_value, parser_parse_statement_or_expr(parser, options));
      }
    }
  }

  if (parser->token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA);
  }

  parser_eat(parser, TOKEN_RBRACE);

  if (ast->list_value->size) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];

      if (child->type == AST_COLON_ASSIGNMENT || (child->type == AST_NAME && !child->flags))
        is_object = 1;
    }
  } else {
    is_object = 1;
  }

  if (!is_object)
    ast->type = AST_SCOPE;

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_function(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_FUNCTION);
  AST_T* ast = init_ast_line(AST_FUNCTION, parser->lexer->line);
  options.parent = ast;

  if (parser->token->type == TOKEN_ID) {
    ast->name = strdup(parser->token->value);
    parser_eat(parser, TOKEN_ID);
  }

  if (parser->token->type == TOKEN_LPAREN) {
    ast->list_value = parse_args(parser, options);
    parser_eat(parser, TOKEN_LBRACE);
  } else {
    ast->type = AST_NAME;
    ast->name = strdup("function");
    return ast;
  }

  if (parser->token->type != TOKEN_RBRACE) {
    ast->body = parser_parse(parser, options);
    if (ast->name) ast->body->name = strdup(ast->name);
  }

  parser_eat(parser, TOKEN_RBRACE);
  
  gc_mark(GC, ast);

  if (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    
    if (ast && ast->name)
    {
      ast_call->name = strdup(ast->name);
    }

    ast_call->left = ast;

    ast = ast_call;
  }


  return ast;
}

AST_T* parser_parse_signature(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_SIGNATURE, parser->lexer->line);
  ast->list_value = parse_args(parser, options);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_factor(parser_T* parser, parser_options_T options)
{
  AST_T* left = 0;

  if (parser->token->type == TOKEN_LPAREN) {
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* expr = parser_parse_expr(parser, options);
    expr->capsulated = 1;
    parser_eat(parser, TOKEN_RPAREN);
    return expr;
  }

  while (parser->token->type == TOKEN_DECREMENT || parser->token->type == TOKEN_INCREMENT || parser->token->type == TOKEN_NOT || parser->token->type == TOKEN_TILDE || parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_MINUS || parser->token->type == TOKEN_SPREAD) {
    AST_T* unop = init_ast_line(AST_UNOP, parser->lexer->line);
    unop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    unop->right = parser_parse_factor(parser, options);
    gc_mark(GC, unop);
    return unop;
  }

  switch (parser->token->type) {
    case TOKEN_ID:
    case TOKEN_CATCH:
    case TOKEN_ELSE:
    case TOKEN_FROM:
    case TOKEN_FOR: return parser_parse_id(parser, options, 1); break;
    case TOKEN_INT: return parser_parse_int(parser, options); break;
    case TOKEN_FLOAT: return parser_parse_float(parser, options); break;
    case TOKEN_STRING: return parser_parse_string(parser, options); break;
    case TOKEN_HEX: return parser_parse_hex(parser, options); break;
    case TOKEN_REGEX: return parser_parse_regex(parser, options); break;
    case TOKEN_LBRACE: return parser_parse_object(parser, options); break;
    case TOKEN_SWITCH: return parser_parse_switch(parser, options); break;
    case TOKEN_DO: return parser_parse_do(parser, options); break;
    case TOKEN_LBRACKET: return parser_parse_array(parser, options); break;
    case TOKEN_FUNCTION: return parser_parse_function(parser, options); break;
    case TOKEN_RETURN:
    case TOKEN_DELETE:
    case TOKEN_EXPORT:
    case TOKEN_TYPEOF:
    case TOKEN_NEW:
    case TOKEN_THROW:
    case TOKEN_BREAK:
    case TOKEN_INSTANCEOF:
    case TOKEN_VOID:
    case TOKEN_ASYNC:
    case TOKEN_AWAIT:
    case TOKEN_ASSERT: return parser_parse_state(parser, options); break;
    case TOKEN_CONST:
    case TOKEN_LET:
    case TOKEN_VAR: return parser_parse_definition(parser, options); break;
    default: {
      return NOOP;
    }; break;
  }
}

AST_T* parser_parse_term(parser_T* parser, parser_options_T options)
{
  AST_T* left = parser_parse_factor(parser, options);
  AST_T* binop = 0;

  if (parser->token->type == TOKEN_EQUALS) {
    AST_T* assignment = parser_parse_assignment(parser, options);
    assignment->left = left;
    if (left && left->name)
    {
      assignment->name = strdup(left->name);
    }
    left = assignment;
  }

  if (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    if (left && left->name)
    {
      ast_call->name = strdup(left->name);
    }
    ast_call->left = left;
    left = ast_call;
  }

  while (parser->token->type == TOKEN_LBRACKET) {
    AST_T* ast_arr = parser_parse_array(parser, options);
    ast_arr->left = left;
    left = ast_arr;
  }

  while (
    parser->token->type == TOKEN_DOT
  ) {
    AST_T* binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);

    binop->right = parser_parse_id(parser, options, 0);

    if (binop->right->name)
      binop->name = strdup(binop->right->name);

    binop->right->from_obj = 1;

    gc_mark(GC, binop);

    left = binop;
  }

  while (
    parser->token->type == TOKEN_PLUS ||
    parser->token->type == TOKEN_MINUS ||
    parser->token->type == TOKEN_EQUALS_EQUALS ||
    parser->token->type == TOKEN_EQUALS_EQUALS_EQUALS ||
    parser->token->type == TOKEN_NOT_EQUALS_EQUALS ||
    parser->token->type == TOKEN_NOT_EQUALS) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);

    gc_mark(GC, binop);

    left = binop;
  }

  while (
    parser->token->type == TOKEN_DIV ||
    parser->token->type == TOKEN_STAR) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;

    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_factor(parser, options);

    gc_mark(GC, binop);

    left = binop;
  } 

  while (
    parser->token->type == TOKEN_AND_AND) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);
    
    gc_mark(GC, binop);

    left = binop;
  }

  while (
    parser->token->type == TOKEN_LT ||
    parser->token->type == TOKEN_LT_EQUALS ||
    parser->token->type == TOKEN_GT ||
    parser->token->type == TOKEN_GT_EQUALS ||
    parser->token->type == TOKEN_PIPE_PIPE ||
    parser->token->type == TOKEN_IN) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);
    
    gc_mark(GC, binop);

    left = binop;
  } 

   while (parser->token->type == TOKEN_QUESTION) {
    parser_eat(parser, TOKEN_QUESTION);
    AST_T* ast_tern = init_ast_line(AST_TERNARY, parser->lexer->line);
    ast_tern->left = left;

    ast_tern->value = parser_parse_expr(parser, options);

    parser_eat(parser, TOKEN_COLON);

    ast_tern->right = parser_parse_expr(parser, options);

    gc_mark(GC, ast_tern);

    left = ast_tern;
  } 

  return left;
}

AST_T* parser_parse_expr(parser_T* parser, parser_options_T options)
{
  AST_T* left = parser_parse_term(parser, options);
  AST_T* binop = 0; 

  while (parser->token->type == TOKEN_ARROW_RIGHT) {
    left->capsulated = 0;
    parser_eat(parser, TOKEN_ARROW_RIGHT);
    AST_T* ast_arrow_def = init_ast_line(AST_ARROW_DEFINITION, parser->lexer->line);
    ast_arrow_def->list_value = init_list(sizeof(AST_T*));
    list_push(ast_arrow_def->list_value, left);

    if (parser->token->type == TOKEN_LBRACE) {
      parser_eat(parser, TOKEN_LBRACE);
      if (parser->token->type != TOKEN_RBRACE) {
        ast_arrow_def->body = parser_parse(parser, options);
      }
      parser_eat(parser, TOKEN_RBRACE);
    } else {
      ast_arrow_def->body = parser_parse_expr(parser, options);
    }

    gc_mark(GC, ast_arrow_def);

    left = ast_arrow_def;
  }

  while (
    parser->token->type == TOKEN_PLUS ||
    parser->token->type == TOKEN_PLUS_EQUALS ||
    parser->token->type == TOKEN_PIPE_EQUALS ||
    parser->token->type == TOKEN_AND_EQUALS ||
    parser->token->type == TOKEN_STAR_EQUALS ||
    parser->token->type == TOKEN_MINUS_EQUALS ||
    parser->token->type == TOKEN_MOD ||
    parser->token->type == TOKEN_SHIFT_RIGHT ||
    parser->token->type == TOKEN_SHIFT_RIGHT_UNSIGNED ||
    parser->token->type == TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS ||
    parser->token->type == TOKEN_AND ||
    parser->token->type == TOKEN_PIPE ||
    parser->token->type == TOKEN_PIPE_PIPE ||
    parser->token->type == TOKEN_MINUS) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_term(parser, options);

    gc_mark(GC, binop);

    left = binop;
  }

  if (parser->token->type == TOKEN_INSTANCEOF) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_term(parser, options);
    
    gc_mark(GC, binop);

    left = binop;
  }  

 while (parser->token->type == TOKEN_LBRACKET) {
    AST_T* ast_arr = parser_parse_array(parser, options);
    ast_arr->left = left;
    left = ast_arr;
  }  

 if (parser->token->type == TOKEN_EQUALS) {
    AST_T* assignment = parser_parse_assignment(parser, options);
    assignment->left = left;
    if (left && left->name)
    {
      assignment->name = strdup(left->name);
    }
    left = assignment;
  }

  while (parser->token->type == TOKEN_COMMA)
  {
    if (left && left->type == AST_DEFINITION)
      break;

    AST_T* tuple = init_ast_line(AST_TUPLE, parser->lexer->line);
    tuple->list_value = parse_tuple(parser, options);
    list_prefix(tuple->list_value, left);
    gc_mark(GC, tuple);
    left = tuple;
  }

  return left;
}

AST_T* parser_parse_case(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_COLON_ASSIGNMENT, parser->lexer->line);
  ast->name = strdup(parser->token->value);
  parser_eat(parser, TOKEN_CASE);
  ast->expr = parser_parse_expr(parser, options);
  parser_eat(parser, TOKEN_COLON);
  ast->right = parser_parse_statement_or_expr(parser, options);

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_statement(parser_T* parser, parser_options_T options)
{
  AST_T* left = 0;

  while (parser->token->type == TOKEN_SEMI)
    parser_eat(parser, TOKEN_SEMI);

  switch (parser->token->type) {
    case TOKEN_IMPORT: return parser_parse_import(parser, options); break;
    case TOKEN_FOR: left = parser_parse_for(parser, options); break;
    case TOKEN_CASE: left = parser_parse_case(parser, options); break;
    case TOKEN_IF:
    case TOKEN_ELSE: left = parser_parse_condition(parser, options); break;
    case TOKEN_TRY: left = parser_parse_try(parser, options); break;
    case TOKEN_WHILE: left = parser_parse_while(parser, options); break;
    case TOKEN_RETURN:
    case TOKEN_DELETE:
    case TOKEN_EXPORT:
    case TOKEN_THROW:
    case TOKEN_BREAK:
    case TOKEN_VOID:
    case TOKEN_ASYNC:
    case TOKEN_AWAIT:
    case TOKEN_ASSERT: return parser_parse_state(parser, options); break;
    default: return 0; break;
  }

  return left;
}

AST_T* parser_parse_statement_or_expr(parser_T* parser, parser_options_T options)
{
  AST_T* left = parser_parse_statement(parser, options);
  left = left ? left : parser_parse_expr(parser, options); 

  if (!left) {
    printf("[Parser] (%s):%d: Unexpected token `%s` (%d)\n", parser->filepath, parser->lexer->line, parser->token->value, parser->token->type);
    exit(1);
  }

  if (left && parser->token->type == TOKEN_COLON) {
    AST_T* colon_ass = init_ast_line(AST_COLON_ASSIGNMENT, parser->lexer->line);
    colon_ass->left = left;
    parser_eat(parser, TOKEN_COLON);
    colon_ass->right = parser_parse_statement_or_expr(parser, options);
    gc_mark(GC, colon_ass);;
    left = colon_ass;
  } 

  return left;
}

AST_T* parser_parse_compound(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_COMPOUND, parser->lexer->line);
  ast->list_value = init_list(sizeof(AST_T*));

  AST_T* child = parser_parse_statement_or_expr(parser, options);
  list_push(ast->list_value, child);

  while (child->type != AST_NOOP && parser->token->type != TOKEN_EOF) {
    if (parser->token->type == TOKEN_SEMI)
      parser_eat(parser, TOKEN_SEMI);

    child = parser_parse_statement_or_expr(parser, options);
    list_push(ast->list_value, child);
  }

  gc_mark(GC, ast);

  return ast;
}

AST_T* parser_parse_call(parser_T* parser, parser_options_T options)
{
  AST_T* ast_call = init_ast_line(AST_CALL, parser->lexer->line);
  ast_call->is_ref = 1;

  ast_call->list_value = parse_args(parser, options);
  ast_call->from_module = strdup(parser->filepath);

  if (parser->token->type == TOKEN_ARROW_RIGHT) {
    parser_eat(parser, TOKEN_ARROW_RIGHT);
    ast_call->type = AST_SIGNATURE;

    if (parser->token->type == TOKEN_LBRACE) {
      parser_eat(parser, TOKEN_LBRACE);
      if (parser->token->type != TOKEN_RBRACE) {
        ast_call->body = parser_parse(parser, options);
      }
      parser_eat(parser, TOKEN_RBRACE);
    } else {
      ast_call->body = parser_parse_statement_or_expr(parser, options);
    }
  }

  gc_mark(GC, ast_call);

  if (parser->token->type == TOKEN_LPAREN || parser->token->type == TOKEN_LBRACKET)
  {
    if (parser->token->type == TOKEN_LPAREN)
      ast_call->right = parser_parse_call(parser, options);
    else
      ast_call->right = parser_parse_array(parser, options);
  }

  return ast_call;
}

void parser_eat(parser_T* parser, int token_type)
{
  if (parser->token->type == token_type) {
    if (parser->token) token_free(parser->token);
    parser->token = lexer_next(parser->lexer);
  } else {
    printf("[Parser] (%s):%d: Unexpected token `%s` (%d), was expecting `%s`\n", parser->filepath, parser->lexer->line, parser->token->value, parser->token->type, token_type_to_str(token_type));
    exit(1);
  }
}

void parser_eat_any(parser_T* parser)
{
  if (parser->token) token_free(parser->token);
  parser->token = lexer_next(parser->lexer);
}

void parser_free(parser_T* parser)
{
  if (parser->token) token_free(parser->token);
  free(parser);
}
