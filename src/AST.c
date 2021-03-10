#include "include/AST.h"
#include "include/gc.h"
#include "include/string_utils.h"
#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST_T* init_ast(int type)
{
  AST_T* ast = calloc(1, sizeof(struct FJB_AST_STRUCT));
  ast->type = type;
  ast->line = 0;
  ast->requirements = init_map(16);
  ast->aliases = init_map(8);

  int x = 2;
  int p = 20;

  return ast;
}

AST_T* ast_apply_builtins(AST_T* ast)
{

  if (ast->type == AST_FUNCTION) {
    if (!ast->map)
      ast->map = init_map(16);

    AST_T* func = init_ast(AST_FUNCTION);
    func->name = strdup("call");
    func->body = ast;
    map_set(ast->map, "call", func);
    map_set(ast->map, "apply", func);
  }
}

AST_T* init_ast_line(int type, int line)
{
  AST_T* ast = init_ast(type);
  ast->line = line;

  return ast;
}

void list_free(gc_T* gc, list_T* list)
{
  if (!list)
    return;

  for (unsigned int i = 0; i < list->size; i++) {
    AST_T* child = (AST_T*)list->items[i];

    if (!ptr_in_list(gc->trash, child))
      gc_mark(gc, child);
  }

  if (list->items)
    free(list->items);

  if (list)
    free(list);
}

void ast_free(AST_T* ast)
{
  if (ast->token) {
    token_free(ast->token);
  }

  if (ast->string_value) {
    free(ast->string_value);
  }

  if (ast->from_module) {
    free(ast->from_module);
  }

  if (ast->compiled_value) {
    //  free(ast->compiled_value);
  }

  if (ast->name) {
    free(ast->name);
  }

  if (ast->headers)
    free(ast->headers);

  if (ast->list_value) {
    list_free_shallow(ast->list_value);
  }

  if (ast->flags) {
    if (ast->flags->items) {
      free(ast->flags->items);
    }

    free(ast->flags);
  }

  free(ast);
  ast = 0;
}

AST_T* init_assignment(char* name, AST_T* value)
{
  AST_T* ast = init_ast(AST_ASSIGNMENT);

  ast->name = name ? strdup(name) : 0;
  ast->left = init_ast(AST_NAME);
  ast->left->name = name ? strdup(name) : 0;
  ast->value = value;

  return ast;
}

AST_T* init_definition(char* name, AST_T* value)
{
  AST_T* assignment = init_assignment(name, value);
  assignment->flags = NEW_STACK;
  list_push(assignment->flags, init_ast_name("var"));
  return assignment;
}

AST_T* init_ast_string(char* string_value)
{
  AST_T* ast = init_ast(AST_STRING);
  ast->string_value = string_value ? strdup(string_value) : strdup(" ");

  return ast;
}

AST_T* init_ast_name(char* name)
{
  AST_T* ast = init_ast(AST_NAME);
  ast->name = name ? strdup(name) : strdup(" ");

  return ast;
}

unsigned int ast_is_callable(AST_T* ast)
{
  if (!ast)
    return 0;
  return ast->type == AST_FUNCTION || ast->type == AST_ARROW_DEFINITION ||
         ast->type == AST_SIGNATURE;
}

char* ast_get_string(AST_T* ast)
{
  if (!ast)
    return 0;

  if (ast->name)
    return ast->name;

  if (ast->alias)
    return ast->alias;

  if (ast->string_value)
    return ast->string_value;

  return 0;
}

char* ast_get_string_copy(AST_T* ast)
{
  if (!ast)
    return 0;

  char* value = ast_get_string(ast);
  if (!value)
    return 0;

  return strdup(value);
}

char* ast_type_to_str(AST_T* ast)
{
  if (!ast)
    return 0;
  return (char*)AST_TYPE_STR[ast->type];
}

list_T* ast_get_parents(AST_T* ast)
{
  list_T* l = NEW_STACK;
  AST_T* p = ast->parent;

  while (p) {
    list_push(l, p);
    p = p->parent;
  }

  return l;
}

char* ast_get_alias(AST_T* ast)
{
  return ast->alias ? ast->alias : ast->name;
}

AST_T* ast_value(AST_T* ast)
{
  if (!ast)
    return 0;
  if (ast->type == AST_FUNCTION)
    return ast;

  if (ast->value)
    return ast->value;
  return ast;
}

AST_T* ast_ptr(AST_T* ast)
{
  if (!ast)
    return 0;

  AST_T* ptr = ast->ptr;
  while (ptr && ptr->ptr && ptr->ptr != ptr && ptr->ptr != ast) {
    ptr = ptr->ptr;
  }

  if (ptr)
    return ptr;

  return ast;
}

unsigned int ast_has_index(AST_T* ast)
{
  if (!ast)
    return 0;
  if (ast->capsulated)
    return 1;
  if (ast->type == AST_FUNCTION)
    return 0;
  if (ast->type == AST_ASSIGNMENT)
    return 0;
  if (ast->type == AST_ARRAY || ast->type == AST_TUPLE)
    return 1;

  return 1;
}
