#include "include/parser.h"
#include "include/emit.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/io.h"
#include "include/jsx.h"
#include "include/node.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern AST_T* NOOP;

parser_T* init_parser(lexer_T* lexer, fjb_env_T* env)
{
  parser_T* parser = calloc(1, sizeof(struct FJB_PARSER_STRUCT));
  parser->lexer = lexer;
  parser->token = lexer_next(lexer);
  parser->env = env;

  return parser;
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
    child->not_exported = options.not_exported;
    list_push(list_value, child);

    while (parser->token->type != TOKEN_RPAREN) {
      if (parser->token->type == TOKEN_SEMI)
        parser_eat(parser, TOKEN_SEMI);

      child = parser_parse_expr(parser, options);
      child->not_exported = options.not_exported;
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
    gc_mark(parser->env->GC, undef);
    list_push(list_value, undef);
  }

  if (parser->token->type != TOKEN_RBRACKET) {
    AST_T* child = parser_parse_expr(parser, options);
    list_push(list_value, child);

    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      child = parser_parse_expr(parser, options);
      list_push(list_value, child);
    }
  }

  parser_eat(parser, TOKEN_RBRACKET);

  return list_value;
}

static AST_T* parse_block_linked_list(parser_T* parser,
                                      int types[],
                                      unsigned int length,
                                      parser_options_T options)
{
  AST_T* left = 0;

  for (unsigned int i = 0; i < length; i++) {
    if (parser->token->type == types[i]) {
      left = init_ast_line(AST_TRY, parser->lexer->line);
      gc_mark(parser->env->GC, left);
      left->parent = options.parent;
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

        if (left->right) {
          left = left->right;
        }
      }
    }
  }

  return left;
}

AST_T* parser_parse_gate(parser_T* parser, parser_options_T options, AST_T* left)
{
  AST_T* binop = left;

  while (parser->token->type == TOKEN_AND_AND) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);

    gc_mark(parser->env->GC, binop);

    left = binop;
  }

  return left;
}

AST_T* parser_parse_dot_notation(parser_T* parser, parser_options_T options, AST_T* left)
{
  AST_T* binop = left;

  while (parser->token->type == TOKEN_DOT) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);

    binop->right = parser_parse_term(parser, options);

    if (binop->left->name)
      binop->name = strdup(binop->left->name);

    binop->from_obj = 1;
    if (left)
      left->from_obj = 1;
    if (binop->right)
      binop->right->from_obj = 1;

    gc_mark(parser->env->GC, binop);
    left = binop;
  }

  return binop;
}

AST_T* parser_parse_ternary(parser_T* parser, parser_options_T options, AST_T* left)
{
  while (parser->token->type == TOKEN_QUESTION) {
    parser_eat(parser, TOKEN_QUESTION);
    AST_T* ast_tern = init_ast_line(AST_TERNARY, parser->lexer->line);
    ast_tern->parent = options.parent;
    ast_tern->left = left;

    ast_tern->value = parser_parse_expr(parser, options);

    /**
     * The reason why we're not expecting a TOKEN_COLON here,
     * is because it's later being taken care of when parsing the
     * next expression.
     */

    ast_tern->right = parser_parse_expr(parser, options);

    gc_mark(parser->env->GC, ast_tern);

    left = ast_tern;
  }

  return left;
}

AST_T* parser_parse(parser_T* parser, parser_options_T options)
{
  return parser_parse_compound(parser, options);
}

AST_T* parser_parse_id(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_NAME, parser->lexer->line);
  ast->parent = options.parent;
  ast->string_value = strdup(parser->token->value);
  ast->name = strdup(ast->string_value);
  ast->from_module = parser->env->filepath ? strdup(parser->env->filepath) : 0;

  parser_eat_any(parser);

  if (parser->token->type == TOKEN_AS) {
    parser_eat(parser, TOKEN_AS);
    ast->alias = strdup(parser->token->value);
    parser_eat(parser, TOKEN_ID);
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_int(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_INT, parser->lexer->line);
  ast->int_value = atoi(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_INT);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_int_min(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_INT_MIN, parser->lexer->line);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_INT_MIN);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_float(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_FLOAT, parser->lexer->line);
  ast->float_value = atof(parser->token->value);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_FLOAT);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_string(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_STRING, parser->lexer->line);
  ast->token = token_clone(parser->token);

  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_STRING);
  ast->name = strdup(ast->string_value);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_hex(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_HEX, parser->lexer->line);

  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_HEX);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_import(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_IMPORT, parser->lexer->line);
  parser_eat(parser, TOKEN_IMPORT);

  ast->list_value = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_ID) {
    AST_T* ast_import_arg = parser_parse_id(parser, options);
    list_push(ast->list_value, ast_import_arg);

    if (parser->token->type == TOKEN_COMMA)
      parser_eat(parser, TOKEN_COMMA);
  }

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);

    AST_T* ast_import_arg = parser_parse_id(parser, options);
    list_push(ast->list_value, ast_import_arg);

    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);
      AST_T* ast_import_arg = parser_parse_id(parser, options);
      list_push(ast->list_value, ast_import_arg);
    }

    parser_eat(parser, TOKEN_RBRACE);
  } else if (parser->token->type == TOKEN_STAR) {
    parser_eat(parser, TOKEN_STAR);

    if (parser->token->type == TOKEN_AS) {
      parser_eat(parser, TOKEN_AS);
      ast->alias = strdup(parser->token->value);
      list_push(ast->list_value, parser_parse_id(parser, options));
    }
  } else if (parser->token->type != TOKEN_STRING && parser->token->type != TOKEN_FROM) {
    AST_T* ast_import_arg = parser_parse_id(parser, options);
    list_push(ast->list_value, ast_import_arg);
  }

  if ((ast->list_value->size > 0) || ast->alias) {
    parser_eat(parser, TOKEN_FROM);
  }
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_STRING);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_definition(parser_T* parser, parser_options_T options)
{
  list_T* flags = init_list(sizeof(AST_T*));

  while (parser->token->type == TOKEN_CONST || parser->token->type == TOKEN_LET ||
         parser->token->type == TOKEN_VAR) {
    AST_T* ast_flag = init_ast_line(AST_NAME, parser->lexer->line);
    ast_flag->string_value = strdup(parser->token->value);
    ast_flag->name = strdup(ast_flag->string_value);
    gc_mark(parser->env->GC, ast_flag);
    list_push(flags, ast_flag);
    parser_eat(parser, parser->token->type);
  }

  AST_T* left = 0;

  if (parser->token->type == TOKEN_LBRACKET) {
    left = parser_parse_array(parser, options);
  } else {
    left = parser_parse_id(parser, options);
  }
  AST_T* assignment = parser_parse_assignment(parser, options, left);
  assignment->flags = flags;

  return assignment;
}

AST_T* parser_parse_assignment(parser_T* parser, parser_options_T options, AST_T* id)
{
  AST_T* ast = init_ast_line(AST_ASSIGNMENT, parser->lexer->line);
  gc_mark(parser->env->GC, ast);
  ast->parent = options.parent;
  ast->left = id;

  if (ast->left && ast->left->name) {
    ast->name = strdup(ast->left->name);
    // list_push(parser->env->search_index, ast);
  }

  AST_T* val = ast->left;
  while (val && parser->token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA);
    val->next = parser_parse_id(parser, options);
    val = val->next;
  }

  if (parser->token->type == TOKEN_EQUALS || parser->token->type == TOKEN_MOD_EQUALS) {
    parser_eat(parser, parser->token->type);

    if (parser->token->type == TOKEN_LBRACE) {
      ast->value = parser_parse_object(parser, options);
    } else {
      ast->value = parser_parse_expr(parser, options);
    }
  }

  if (parser->token->type == TOKEN_COMMA) {
    list_T* tuple = parse_tuple(parser, options);
    list_prefix(tuple, ast);
    AST_T* tuple_ast = init_ast(AST_TUPLE);
    tuple_ast->list_value = tuple;
    return tuple_ast;
  }
  return ast;
}

AST_T* parser_parse_state(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_STATE, parser->lexer->line);
  ast->parent = options.parent;
  ast->token = token_clone(parser->token);
  ast->string_value = strdup(parser->token->value);
  ast->name = strdup(ast->string_value ? ast->string_value : ast->name);

  parser_eat(parser, parser->token->type);

  if (ast->token->type != TOKEN_DEFAULT) {
    if (parser->token->type == TOKEN_COLON) {
      ast->type = AST_NAME;
      return ast;
    }
  }

  if (parser->token->type != TOKEN_SEMI) {
    ast->value = parser_parse_expr(parser, options);

    if (ast->value && ast->token->type == TOKEN_EXPORT)
      ast->value->exported = 1;
  }

  gc_mark(parser->env->GC, ast);

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

  if (parser->token->type == TOKEN_WHILE) {
    if (ast->right)
      ast->right->right = parser_parse_statement_or_expr(parser, options);
    else
      ast->right = parser_parse_statement_or_expr(parser, options);
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_class(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_CLASS, parser->lexer->line);
  ast->token = token_clone(parser->token);
  ast->options = NEW_STACK;
  parser_eat(parser, TOKEN_CLASS);

  ast->name = strdup(parser->token->value);
  parser_eat(parser, TOKEN_ID);

  // extends ... etc
  while (parser->token->type != TOKEN_LBRACE) {
    AST_T* statement = parser_parse_statement(parser, options);
    list_push(ast->options, statement);
  }

  parser_eat(parser, TOKEN_LBRACE);
  ast->body = parser_parse(parser, options);
  parser_eat(parser, TOKEN_RBRACE);

  gc_mark(parser->env->GC, ast);

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

  if (parser->token->type == TOKEN_ELSE) {
    ast->right = parser_parse_condition(parser, options);
    ast->right->prev = ast;
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_do(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_DO, parser->lexer->line);
  parser_eat(parser, TOKEN_DO);

  if (parser->token->type == TOKEN_IF) {
    ast->condition = parser_parse_condition(parser, options);
    return ast;
  }

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = parser_parse(parser, options);
    }

    parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->body = parser_parse_expr(parser, options);
  }

  if (parser->token->type == TOKEN_SEMI)
    parser_eat(parser, TOKEN_SEMI);

  if (parser->token->type == TOKEN_WHILE) {
    ast->right = parser_parse_while(parser, options);
  }

  gc_mark(parser->env->GC, ast);

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

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_for(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_FOR);
  AST_T* ast = init_ast_line(AST_FOR, parser->lexer->line);
  options.not_exported = 1;
  ast->list_value = parse_semi_args(parser, options);
  options.not_exported = 0;

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);
    ast->body = parser_parse(parser, options);
    parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->body = parser_parse_statement_or_expr(parser, options);
  }

  if (ast->list_value->size == 2)
    list_push(ast->list_value, init_ast(AST_NOOP));

  gc_mark(parser->env->GC, ast);

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

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_array(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_ARRAY, parser->lexer->line);
  ast->parent = options.parent;
  ast->list_value = parse_array(parser, options);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_regex(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_REGEX, parser->lexer->line);
  ast->string_value = strdup(parser->token->value);
  parser_eat(parser, TOKEN_REGEX);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_scope(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_SCOPE, parser->lexer->line);
  ast->list_value = init_list(sizeof(token_T*));
  options.parent = ast;

  if (parser->token->type == TOKEN_LBRACE) {
    parser_eat(parser, TOKEN_LBRACE);
    ast->body = parser_parse(parser, options);
    parser_eat(parser, TOKEN_RBRACE);
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_object_child(parser_T* parser, parser_options_T options)
{
  AST_T* colon_ass = init_ast_line(AST_COLON_ASSIGNMENT, parser->lexer->line);
  colon_ass->parent = options.parent;

  colon_ass->left = parser_parse_factor(parser, options);

  if (parser->token->type != TOKEN_COLON) {
    return colon_ass->left;
  }

  parser_eat(parser, TOKEN_COLON);

  colon_ass->right = parser_parse_expr(parser, options);

  if (colon_ass->left) {
    char* name = ast_get_string(colon_ass->left);

    if (name)
      colon_ass->name = strdup(name);
  }

  gc_mark(parser->env->GC, colon_ass);
}

AST_T* parser_parse_object(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_OBJECT, parser->lexer->line);
  ast->list_value = init_list(sizeof(token_T*));
  options.parent = ast;

  parser_eat(parser, TOKEN_LBRACE);

  if (parser->token->type != TOKEN_RBRACE) {
    AST_T* child = parser_parse_object_child(parser, options);
    list_push(ast->list_value, child);

    while (parser->token->type == TOKEN_COMMA) {
      parser_eat(parser, TOKEN_COMMA);

      if (parser->token->type != TOKEN_RBRACE) {
        AST_T* child = parser_parse_object_child(parser, options);
        list_push(ast->list_value, child);
      }
    }
  }

  parser_eat(parser, TOKEN_RBRACE);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_jsx_element(parser_T* parser, parser_options_T options)
{
  return parse_jsx(parser, options);
}

AST_T* parser_parse_function(parser_T* parser, parser_options_T options)
{
  parser_eat(parser, TOKEN_FUNCTION);
  AST_T* ast = init_ast_line(AST_FUNCTION, parser->lexer->line);
  ast->parent = options.parent;
  ast->from_module = parser->env->filepath ? strdup(parser->env->filepath) : 0;
  options.parent = ast;

  if (parser->token->type != TOKEN_LPAREN) {
    ast->name = strdup(parser->token->value);
    parser_eat_any(parser);
  }

  if (parser->token->type == TOKEN_LPAREN) {
    ast->list_value = parse_args(parser, options);
    parser_eat(parser, TOKEN_LBRACE);
  }

  if (parser->token->type != TOKEN_RBRACE) {
    ast->body = parser_parse(parser, options);
    if (ast->name)
      ast->body->name = strdup(ast->name);
  }

  parser_eat(parser, TOKEN_RBRACE);

  if (!ast->name) {
    ast->anon = 1;
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_signature(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_SIGNATURE, parser->lexer->line);
  ast->list_value = parse_args(parser, options);

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_factor(parser_T* parser, parser_options_T options)
{
  AST_T* left = 0;

  if (parser->token->type == TOKEN_LPAREN) {
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* expr = parser_parse_expr(parser, options);
    expr->parent = options.parent;
    expr->capsulated = 1;
    expr->not_exported = 1;
    parser_eat(parser, TOKEN_RPAREN);
    left = expr;

    while (parser->token->type == TOKEN_LPAREN) {
      AST_T* ast_call = parser_parse_call(parser, options);
      ast_call->parent = options.parent;
      if (left && left->name) {
        ast_call->name = strdup(left->name);
      }
      ast_call->left = left;
      left = ast_call;
    }
  }

  while (left && parser->token->type == TOKEN_ARROW_RIGHT) {
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

    gc_mark(parser->env->GC, ast_arrow_def);

    left = ast_arrow_def;
  }

  if (left)
    return left;

  if (parser->token->type == TOKEN_DECREMENT || parser->token->type == TOKEN_INCREMENT ||
      parser->token->type == TOKEN_NOT || parser->token->type == TOKEN_TILDE ||
      parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_MINUS ||
      parser->token->type == TOKEN_SPREAD) {
    AST_T* unop = init_ast_line(AST_UNOP, parser->lexer->line);
    unop->parent = options.parent;
    unop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    unop->right = parser_parse_expr(parser, options);
    gc_mark(parser->env->GC, unop);
    left = unop;
    return left;
  }

  switch (parser->token->type) {
    case TOKEN_ID:
    case TOKEN_CATCH:
    case TOKEN_ELSE:
    case TOKEN_FROM:
    case TOKEN_ASYNC:
    case TOKEN_AS:
    case TOKEN_DEFAULT:
    case TOKEN_CLASS:
    case TOKEN_IN:
    case TOKEN_FOR: left = parser_parse_id(parser, options); break;
    case TOKEN_INT: left = parser_parse_int(parser, options); break;
    case TOKEN_INT_MIN: left = parser_parse_int_min(parser, options); break;
    case TOKEN_FLOAT: left = parser_parse_float(parser, options); break;
    case TOKEN_STRING: left = parser_parse_string(parser, options); break;
    case TOKEN_HEX: left = parser_parse_hex(parser, options); break;
    case TOKEN_REGEX: left = parser_parse_regex(parser, options); break;
    case TOKEN_LBRACE: left = parser_parse_object(parser, options); break;
    case TOKEN_SWITCH: left = parser_parse_switch(parser, options); break;
    case TOKEN_DO: left = parser_parse_do(parser, options); break;
    case TOKEN_LBRACKET: left = parser_parse_array(parser, options); break;
    case TOKEN_FUNCTION: left = parser_parse_function(parser, options); break;
    case TOKEN_LT: left = parser_parse_jsx_element(parser, options); break;
    case TOKEN_RETURN:
    case TOKEN_DELETE:
    case TOKEN_EXPORT:
    case TOKEN_TYPEOF:
    case TOKEN_NEW:
    case TOKEN_THROW:
    case TOKEN_BREAK:
    case TOKEN_INSTANCEOF:
    case TOKEN_VOID:
    case TOKEN_AWAIT:
    case TOKEN_ASSERT: left = parser_parse_state(parser, options); break;
    case TOKEN_CONST:
    case TOKEN_LET:
    case TOKEN_VAR: left = parser_parse_definition(parser, options); break;
    default: {
      left = NOOP;
    }; break;
  }

  while (left && parser->token->type == TOKEN_ARROW_RIGHT) {
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

    gc_mark(parser->env->GC, ast_arrow_def);

    left = ast_arrow_def;
  }

  while (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    ast_call->parent = options.parent;
    if (left && left->name) {
      ast_call->name = strdup(left->name);
    }
    ast_call->left = left;
    left = ast_call;
  }

  while (parser->token->type == TOKEN_LBRACKET) {
    AST_T* ast_arr = parser_parse_array(parser, options);
    ast_arr->parent = options.parent;
    ast_arr->left = left;
    left = ast_arr;
  }

  while (parser->token->type == TOKEN_LPAREN) {
    AST_T* ast_call = parser_parse_call(parser, options);
    ast_call->parent = options.parent;
    if (left && left->name) {
      ast_call->name = strdup(left->name);
    }
    ast_call->left = left;
    left = ast_call;
  }

  return left;
}

AST_T* parser_parse_term(parser_T* parser, parser_options_T options)
{
  AST_T* left = parser_parse_factor(parser, options);
  AST_T* binop = 0;

  while (parser->token->type == TOKEN_LBRACKET) {
    AST_T* ast_arr = parser_parse_array(parser, options);
    ast_arr->parent = options.parent;
    ast_arr->left = left;
    left = ast_arr;
  }

  if (parser->token->type == TOKEN_INSTANCEOF) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_factor(parser, options);

    gc_mark(parser->env->GC, binop);
    return binop;
  }

  while (parser->token->type == TOKEN_DIV || parser->token->type == TOKEN_STAR ||
         parser->token->type == TOKEN_MINUS || parser->token->type == TOKEN_SQUARED) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;

    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_factor(parser, options);

    gc_mark(parser->env->GC, binop);

    left = binop;
  }

  if (parser->token->type == TOKEN_DECREMENT || parser->token->type == TOKEN_INCREMENT) {
    AST_T* unop = init_ast_line(AST_UNOP, parser->lexer->line);
    unop->parent = options.parent;
    unop->left = left;
    unop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    gc_mark(parser->env->GC, unop);
    left = unop;
  }

  return left;
}

AST_T* parser_parse_expr(parser_T* parser, parser_options_T options)
{
  AST_T* left = parser_parse_term(parser, options);
  AST_T* binop = 0;

  if (!left->parent)
    left->parent = options.parent;

  if (parser->token->type == TOKEN_DOT) {
    left = parser_parse_dot_notation(parser, options, left);

    if (!left->parent)
      left->parent = options.parent;
  }

  while (parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_PLUS_EQUALS ||
         parser->token->type == TOKEN_DIV_EQUALS || parser->token->type == TOKEN_PIPE_EQUALS ||
         parser->token->type == TOKEN_AND_EQUALS || parser->token->type == TOKEN_STAR_EQUALS ||
         parser->token->type == TOKEN_MINUS_EQUALS || parser->token->type == TOKEN_MOD ||
         parser->token->type == TOKEN_SHIFT_RIGHT || parser->token->type == TOKEN_SHIFT_LEFT ||
         parser->token->type == TOKEN_BITWISE_SHIFT_RIGHT_UNSIGNED ||
         parser->token->type == TOKEN_SHIFT_RIGHT ||
         parser->token->type == TOKEN_UNSIGNED_SHIFT_RIGHT_ASSIGNMENT ||
         parser->token->type == TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS ||
         parser->token->type == TOKEN_AND || parser->token->type == TOKEN_PIPE ||
         parser->token->type == TOKEN_PIPE_PIPE || parser->token->type == TOKEN_MINUS ||
         parser->token->type == TOKEN_AND_AND) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);

    gc_mark(parser->env->GC, binop);
    return binop;
  }

  while (parser->token->type == TOKEN_LT || parser->token->type == TOKEN_LT_EQUALS ||
         parser->token->type == TOKEN_GT || parser->token->type == TOKEN_GT_EQUALS ||
         parser->token->type == TOKEN_PIPE_PIPE || parser->token->type == TOKEN_IN ||
         parser->token->type == TOKEN_EQUALS_EQUALS ||
         parser->token->type == TOKEN_EQUALS_EQUALS_EQUALS ||
         parser->token->type == TOKEN_NOT_EQUALS ||
         parser->token->type == TOKEN_NOT_EQUALS_EQUALS) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->parent = options.parent;
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, parser->token->type);
    binop->right = parser_parse_expr(parser, options);

    gc_mark(parser->env->GC, binop);
    return binop;
  }

  if (parser->token->type == TOKEN_QUESTION) {
    left = parser_parse_ternary(parser, options, left);
  }

  if (parser->token->type == TOKEN_EQUALS || parser->token->type == TOKEN_MOD_EQUALS) {
    return parser_parse_assignment(parser, options, left);
  }

  while (parser->token->type == TOKEN_COMMA && (left->parent && left->parent->type != AST_OBJECT)) {
    binop = init_ast_line(AST_BINOP, parser->lexer->line);
    binop->left = left;
    binop->token = token_clone(parser->token);
    parser_eat(parser, TOKEN_COMMA);
    binop->right = parser_parse_expr(parser, options);
    left = binop;
  }

  if (left && parser->token->type == TOKEN_COLON && options.stop_token != parser->token->type) {
    AST_T* colon_ass = init_ast_line(AST_COLON_ASSIGNMENT, parser->lexer->line);
    colon_ass->parent = options.parent;
    colon_ass->left = left;

    parser_eat(parser, TOKEN_COLON);

    colon_ass->right = parser_parse_statement_or_expr(parser, options);

    if (colon_ass->left) {
      char* name = ast_get_string(colon_ass->left);

      if (name)
        colon_ass->name = strdup(name);
    }

    // list_push(parser->env->search_index, colon_ass);

    gc_mark(parser->env->GC, colon_ass);
    left = colon_ass;
  }

  return left;
}

AST_T* parser_parse_case(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_CASE, parser->lexer->line);
  ast->parent = options.parent;
  ast->name = strdup(parser->token->value);
  parser_eat(parser, TOKEN_CASE);

  options.stop_token = TOKEN_COLON;
  ast->condition = parser_parse_expr(parser, options);
  parser_eat(parser, TOKEN_COLON);
  options.stop_token = 0;

  /**
   * The reason why we're not expecting a TOKEN_COLON here,
   * is because it's being taken care of later by the next
   * parsing of expression.
   */

  ast->value = parser_parse_statement_or_expr(parser, options);

  gc_mark(parser->env->GC, ast);

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
    case TOKEN_CLASS: left = parser_parse_class(parser, options); break;
    case TOKEN_WHILE: left = parser_parse_while(parser, options); break;
    case TOKEN_LBRACE: left = parser_parse_scope(parser, options); break;
    case TOKEN_RETURN:
    case TOKEN_DELETE:
    case TOKEN_EXPORT:
    case TOKEN_THROW:
    case TOKEN_BREAK:
    case TOKEN_VOID:
    case TOKEN_ASYNC:
    case TOKEN_AWAIT:
    case TOKEN_DEFAULT:
    case TOKEN_EXTENDS:
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
    printf("[Parser] (%s):%d: Unexpected token `%s` (%d)\n",
           parser->env->filepath,
           parser->lexer->line,
           parser->token->value,
           parser->token->type);
    exit(1);
  }

  if (parser->token->type == TOKEN_COMMA) {
    list_T* tuple = parse_tuple(parser, options);
    list_prefix(tuple, left);
    AST_T* tuple_ast = init_ast(AST_TUPLE);
    tuple_ast->list_value = tuple;
    return tuple_ast;
  }

  if (!left->parent)
    left->parent = options.parent;

  return left;
}

AST_T* parser_parse_compound(parser_T* parser, parser_options_T options)
{
  AST_T* ast = init_ast_line(AST_COMPOUND, parser->lexer->line);

  ast->parent = options.parent;
  ast->list_value = init_list(sizeof(AST_T*));

  if (!options.parent)
    options.parent = ast;

  AST_T* child = parser_parse_statement_or_expr(parser, options);
  list_push(ast->list_value, child);

  while (child->type != AST_NOOP && parser->token->type != TOKEN_EOF) {
    if (parser->token->type == TOKEN_SEMI)
      parser_eat(parser, TOKEN_SEMI);

    child = parser_parse_statement_or_expr(parser, options);

    if (!child->parent)
      child->parent = options.parent;

    list_push(ast->list_value, child);
  }

  gc_mark(parser->env->GC, ast);

  return ast;
}

AST_T* parser_parse_call(parser_T* parser, parser_options_T options)
{
  AST_T* ast_call = init_ast_line(AST_CALL, parser->lexer->line);
  ast_call->parent = options.parent;
  ast_call->list_value = parse_args(parser, options);
  ast_call->from_module = parser->env->filepath ? strdup(parser->env->filepath) : 0;

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

  if (parser->token->type == TOKEN_LBRACE) {
    ast_call->type = AST_CLASS_FUNCTION;
    if (parser->token->type == TOKEN_LBRACE) {
      parser_eat(parser, TOKEN_LBRACE);
      if (parser->token->type != TOKEN_RBRACE) {
        ast_call->body = parser_parse(parser, options);
      }
      parser_eat(parser, TOKEN_RBRACE);
    }
  }

  gc_mark(parser->env->GC, ast_call);

  return ast_call;
}

void parser_eat(parser_T* parser, int token_type)
{
  if (parser->token && parser->token->type == token_type) {
    token_free(parser->token);
    parser->token = lexer_next(parser->lexer);
  } else {
    printf("[Parser] (%s):%d: Unexpected token `%s` (%d), was expecting `%s`\n",
           parser->env->filepath,
           parser->lexer->line,
           parser->token->value,
           parser->token->type,
           token_type_to_str(token_type));
    exit(1);
  }
}

void parser_eat_any(parser_T* parser)
{
  if (parser->token)
    token_free(parser->token);
  parser->token = lexer_next(parser->lexer);
}

void parser_free(parser_T* parser)
{
  if (parser->token)
    token_free(parser->token);
  free(parser);
}
