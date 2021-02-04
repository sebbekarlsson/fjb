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

  return ast;
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
    free(ast->compiled_value);
  }

  if (ast->name) {
    free(ast->name);
  }

  if (ast->headers)
    free(ast->headers);

  // if (ast->node)
  //  ast_free(ast->node);

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

void ast_get_pointers(list_T* list, AST_T* ast)
{
  if (!ast)
    return;

  AST_T* ptr = ast->ptr;

  if (ptr)
    list_push(list, ptr);

  ast_get_pointers(list, ptr);
}

AST_T* ast_get_final_ptr(AST_T* ast)
{
  if (!ast)
    return 0;
  if (ast->value)
    return ast_get_final_ptr(ast->value);
  if (ast->ptr)
    return ast->ptr;
  return ast;
}

AST_T* get_node_by_name(list_T* list, char* name)
{
  if (!name)
    return 0;

  LOOP_NODES(list, i, child, {
    char* child_name = ast_get_string(child);
    if (!child_name)
      continue;

    if (child_name == name || (strcmp(child_name, name) == 0))
      return child;
  });

  return 0;
}

AST_T* ast_query(list_T* list, unsigned int (*match)(AST_T* ast, query_T query), query_T query)
{
  LOOP_NODES(list, i, child, {
    if (match(child, query))
      return child;
  });

  return 0;
}

char* ast_encode_strings(list_T* strings)
{
  char* str = 0;

  LOOP_NODES(strings, i, ast, {
    if (!ast->string_value && !ast->name)
      continue;

    char* v = ast->string_value ? ast->string_value : ast->name;

    char* enc = str_encode(v);
    str = str_append(&str, enc);
    free(enc);
  });

  return str;
}

char* ast_get_string(AST_T* ast)
{
  if (!ast)
    return 0;

  if (ast->alias)
    return ast->alias;

  if (ast->name)
    return ast->name;

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

unsigned int ast_is_iterable(AST_T* ast)
{
  if (!ast)
    return 0;

  if (ast->type == AST_CALL && ast->name && strcmp(ast->name, "map") == 0)
    return 1;
  if (ast->type == AST_ARRAY || ast->type == AST_TUPLE)
    return 1;

  if (ast_is_iterable(ast->value))
    return 1;
  if (ast_is_iterable(ast->left))
    return 1;
  if (ast_is_iterable(ast->right))
    return 1;

  return 0;
}
