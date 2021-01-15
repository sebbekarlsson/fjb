#include "include/mkalive.h"
#include <stdio.h>
#include <string.h>

list_T* mkalive_list(visitor_T* visitor, list_T* list, list_T* args, list_T* stack)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    AST_T* child = (AST_T*) list->items[i];
    if (!child) continue;

    list->items[i] = ALIVE(child); 
  }

  return list;
}

static AST_T* mkalive_array(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->list_value) ALIVE_LIST(ast->list_value);
  ast->alive = 1;

  return ast;
}

static AST_T* mkalive_arrow_definition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value) ALIVE_LIST(ast->list_value);
  if (ast->body) ALIVE(ast->body);

  return ast;
}

static AST_T* mkalive_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  if (ast->value) ALIVE(ast->value);
  if (ast->list_value) ALIVE_LIST(ast->list_value);
  if (ast->expr) ALIVE(ast->expr);

  return ast;
}

static AST_T* mkalive_definition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->list_value)
  {
    list_T* list_value = get_living_nodes(ast->list_value);
    
    if (ast->list_value->items) free(ast->list_value->items);
    free(ast->list_value);

    ast->list_value = list_value;
  }

  ast->alive = 1;
  if (ast->flags) ALIVE_LIST(ast->flags);
  if (ast->list_value) ALIVE_LIST(ast->list_value);
  if (ast->expr) ALIVE(ast->expr);

  return ast;
}

static AST_T* mkalive_colon_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_while(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->expr)
  {
    ALIVE(ast->expr);
  }

  if (ast->body)
  {
    ALIVE(ast->body);
  }

  return ast;
}

static AST_T* mkalive_for(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  ast->list_value = ALIVE_LIST(ast->list_value); 

  if (ast->body)
  {
    ALIVE(ast->body);
  }

  return ast;
}

static AST_T* mkalive_compound(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  ast->list_value = mkalive_list(visitor, ast->list_value, args, stack);

  return ast;
}

static AST_T* mkalive_import(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

static AST_T* mkalive_undefined(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

static AST_T* mkalive_call(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value)
  {
    ALIVE_LIST(ast->list_value);
  }

  if (ast->body)
  {
    ALIVE(ast->body);
  }

  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_function(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value)
  {
    ALIVE_LIST(ast->list_value);
  }

  if (ast->body)
  {
    ALIVE(ast->body);
  }

  return ast;
}

static AST_T* mkalive_signature(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value)
  {
    ALIVE_LIST(ast->list_value);
  }

  if (ast->body)
  {
    ALIVE(ast->body);
  }

  return ast;
}

static AST_T* mkalive_name(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

static AST_T* mkalive_state(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->value) ALIVE(ast->value);

  return ast;
}

static AST_T* mkalive_binop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_unop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_increment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_decrement(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);

  return ast;
}

static AST_T* mkalive_regex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

static AST_T* mkalive_object(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value)
  {
    ALIVE_LIST(ast->list_value);
  }

  return ast;
}

static AST_T* mkalive_condition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  if (ast->body) ALIVE(ast->body);

  return ast;
}

static AST_T* mkalive_switch(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->expr) ALIVE(ast->expr);
  if (ast->body) ALIVE(ast->body);

  return ast;
}

static AST_T* mkalive_try(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  if (ast->body) ALIVE(ast->body);
  if (ast->list_value) ALIVE_LIST(ast->list_value);

  return ast;
}

static AST_T* mkalive_ternary(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  if (ast->value) ALIVE(ast->value);

  return ast;
}

static AST_T* mkalive_do(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->body) ALIVE(ast->body);

  return ast;
}

static AST_T* mkalive_noop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

static AST_T* mkalive_tuple(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value) ALIVE_LIST(ast->list_value);

  return ast;
}

AST_T* mkalive(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (!ast) return 0;
  if (ast->alive) return ast;

  if (ast->body && ast->body->alive) ast->body->alive = 0;

  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  if (ast->value) ALIVE(ast->value);

  if (ast->expr) ALIVE(ast->expr);
  if (ast->body) ALIVE(ast->body);
  if (ast->source_ast && !ast->source_ast->alive) ALIVE(ast->source_ast);

  list_T* pointers = ast_get_pointers(ast);

  for (unsigned int i = 0; i < pointers->size; i++)
  {
    AST_T* ptr = (AST_T*) pointers->items[i];
    if (!ptr) continue;
    if (ptr == ast) continue;

    ALIVE(ptr);
  }

  if (pointers)
  {
    if (pointers->items)
      free(pointers->items);

    free(pointers);
  }

  switch (ast->type) {
    case AST_ARRAY: return mkalive_array(visitor, ast, args, stack); break;
    case AST_ARROW_DEFINITION: return mkalive_arrow_definition(visitor, ast, args, stack); break;
    case AST_ASSIGNMENT: return mkalive_assignment(visitor, ast, args, stack); break;
    case AST_DEFINITION: return mkalive_definition(visitor, ast, args, stack); break;
    case AST_COLON_ASSIGNMENT: return mkalive_colon_assignment(visitor, ast, args, stack); break;
    case AST_WHILE: return mkalive_while(visitor, ast, args, stack); break;
    case AST_FOR: return mkalive_for(visitor, ast, args, stack); break;
    case AST_COMPOUND: return mkalive_compound(visitor, ast, args, stack); break;
    case AST_IMPORT: return mkalive_import(visitor, ast, args, stack); break;
    case AST_UNDEFINED: return mkalive_undefined(visitor, ast, args, stack); break;
    case AST_CALL: return mkalive_call(visitor, ast, args, stack); break;
    case AST_FUNCTION: return mkalive_function(visitor, ast, args, stack); break;
    case AST_SIGNATURE: return mkalive_signature(visitor, ast, args, stack); break;
    case AST_NAME: return mkalive_name(visitor, ast, args, stack); break;
    case AST_STATE: return mkalive_state(visitor, ast, args, stack); break;
    case AST_BINOP: return mkalive_binop(visitor, ast, args, stack); break;
    case AST_UNOP: return mkalive_unop(visitor, ast, args, stack); break;
    case AST_INCREMENT: return mkalive_increment(visitor, ast, args, stack); break;
    case AST_DECREMENT: return mkalive_decrement(visitor, ast, args, stack); break;
    case AST_REGEX: return mkalive_regex(visitor, ast, args, stack); break;
    case AST_OBJECT: return mkalive_object(visitor, ast, args, stack); break;
    case AST_CONDITION: return mkalive_condition(visitor, ast, args, stack); break;
    case AST_SWITCH: return mkalive_switch(visitor, ast, args, stack); break;
    case AST_TRY: return mkalive_try(visitor, ast, args, stack); break;
    case AST_TERNARY: return mkalive_ternary(visitor, ast, args, stack); break;
    case AST_DO: return mkalive_do(visitor, ast, args, stack); break;
    case AST_NOOP: return mkalive_noop(visitor, ast, args, stack); break;
    case AST_TUPLE: return mkalive_tuple(visitor, ast, args, stack); break;
    default: {
      ast->alive = 1;
      return ast;
    } break;
  }
}
