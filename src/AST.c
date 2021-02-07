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

AST_T* init_ast_string(char* string_value)
{
  AST_T* ast = init_ast(AST_STRING);
  ast->string_value = string_value ? strdup(string_value) : strdup(" ");

  return ast;
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
