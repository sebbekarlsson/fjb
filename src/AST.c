#include "include/AST.h"
#include "include/string_utils.h"
#include "include/token.h"
#include "include/gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST_T* init_ast(int type)
{
  AST_T* ast = calloc(1, sizeof(struct FJB_AST_STRUCT));
  ast->type = type;
  ast->expr = 0;
  ast->list_value = 0;
  ast->value = 0;
  ast->left = 0;
  ast->right = 0;

  return ast;
}

char* ast_binop_to_str(AST_T* ast)
{
  char* left = ast_to_str(ast->left);
  char* tok = token_to_str(ast->token);
  char* right = ast_to_str(ast->right);

  const char* template = "BINOP(%s -> %s -> %s)";
  char* value =
    calloc(strlen(template) + strlen(left) + strlen(tok) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, tok, right);

  free(left);
  free(tok);
  free(right);

  return value;
}

char* ast_name_to_str(AST_T* ast)
{
  const char* template = "NAME(%s)";
  char* value = calloc(strlen(template) + strlen(ast->name) + 1, sizeof(char));
  sprintf(value, template, ast->name);

  return value;
}

char* ast_string_to_str(AST_T* ast)
{
  const char* template = "STRING(%s)";
  char* value = calloc(strlen(template) + strlen(ast->string_value) + 1, sizeof(char));
  sprintf(value, template, ast->string_value);

  return value;
}

char* ast_int_to_str(AST_T* ast)
{
  const char* template = "INT(%d)";
  char* value = calloc(strlen(template) + 32, sizeof(char));
  sprintf(value, template, ast->int_value);

  return value;
}

char* ast_arrow_definition_to_str(AST_T* ast)
{
  const char* template = "ARROW_DEFINITION(%s)";
  char* value = calloc(strlen(template), sizeof(char));
  sprintf(value, template, "");

  return value;
}

char* ast_call_to_str(AST_T* ast)
{
  const char* template = "//%s\n%s.CALL(%s)";
  char* name = ast->left ? ast_to_str(ast->left) : "UNKNOWN";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",\n");
  }
  
  char* value = calloc(strlen(template) + strlen(name) + strlen(str) + 1 + (ast->from_module ? strlen(ast->from_module) : 1) + 256, sizeof(char));

  sprintf(value, template, ast->from_module ? ast->from_module : "", name, str ? str : "");

  return value;
}

char* ast_assignment_to_str(AST_T* ast)
{
  const char* template = "ASSIGNMENT(%s = %s)";
  char* name = ast->left ? ast_to_str(ast->left) : ast->name ? ast->name : "UNKNOWN";
  char* value = calloc(strlen(template) + strlen(name) + 1, sizeof(char));

  char* val = ast->value ? ast_to_str(ast->value) : "(nil)";
  sprintf(value, template, name, val);

  return value;
}

char* ast_object_to_str(AST_T* ast)
{
  const char* template = "OBJECT(%s)";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",");
  }

  str = str ? str : strdup("");

  char* value = calloc(strlen(template) + strlen(str) + 1, sizeof(char));
  sprintf(value, template, str);

  return value;
}

char* ast_tuple_to_str(AST_T* ast)
{
  const char* template = "TUPLE(%s)";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",");
  }

  str = str ? str : strdup("");

  char* value = calloc(strlen(template) + strlen(str) + 1, sizeof(char));
  sprintf(value, template, str);

  return value;
}

char* ast_function_to_str(AST_T* ast)
{
  const char* template = "FUNCTION %s(%s) {\n";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",\n");
  }

  str = str ? str : strdup("");

  char* value = calloc(strlen(template) + strlen(str) + 1, sizeof(char));
  sprintf(value, template, ast->name, str);

  if (ast->body) {
    value = str_append(&value, ast_to_str(ast->body));
  }
  value = str_append(&value, "\n}");

  return value;
}

char* ast_array_to_str(AST_T* ast)
{
  const char* template = "ARRAY(%s)";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",");
  }

  str = str ? str : strdup("");

  char* value = calloc(strlen(template) + strlen(str) + 1, sizeof(char));
  sprintf(value, template, str);

  return value;
}

char* ast_compound_to_str(AST_T* ast)
{
  const char* template = "COMPOUND(%s)";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",\n");
  }

  str = str ? str : strdup("");

  char* value = calloc(strlen(template) + strlen(str) + 1, sizeof(char));
  sprintf(value, template, str);

  return value;
}

char* unstringable(AST_T* ast)
{
  const char* template = "UNSTRINGABLE(%d)";
  char* value = calloc(strlen(template) + 32, sizeof(char));
  sprintf(value, template, ast ? (int)ast->type : 0);

  return value;
}

char* ast_to_str(AST_T* ast)
{
  if (!ast)
    return strdup("AST(nil)");

  switch (ast->type) {
    case AST_BINOP: return ast_binop_to_str(ast); break;
    case AST_NAME: return ast_name_to_str(ast); break;
    case AST_INT: return ast_int_to_str(ast); break;
    case AST_ARROW_DEFINITION: return ast_arrow_definition_to_str(ast); break;
    case AST_CALL: return ast_call_to_str(ast); break;
    case AST_OBJECT: return ast_object_to_str(ast); break;
    case AST_ASSIGNMENT: return ast_assignment_to_str(ast); break;
    case AST_TUPLE: return ast_tuple_to_str(ast); break;
    case AST_ARRAY: return ast_array_to_str(ast); break;
    case AST_STRING: return ast_string_to_str(ast); break;
    case AST_FUNCTION: return ast_function_to_str(ast); break;
    case AST_COMPOUND: return ast_compound_to_str(ast); break;
    default: return unstringable(ast); break;
  }
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
  /*if (ast->expr)
  {
    ast_free(ast->expr, freed);
  }*/

  /*
  if (ast->value)
  {
    ast_free(ast->value, freed);
  }*/
 
  /*if (ast->ref)
  {
    ast_free(ast->ref, freed);
  }

  if (ast->body)
  {
    ast_free(ast->body, freed);
  }

  if (ast->body2)
  {
    ast_free(ast->body2, freed);
  }

  if (ast->access)
  {
    ast_free(ast->access, freed);
  }

  if (ast->label_value)
  {
    ast_free(ast->label_value, freed);
  }

  if (ast->phony_value)
  {
    ast_free(ast->phony_value, freed);
  }

  if (ast->ptr)
  {
    ast_free(ast->ptr, freed);
  }*/

  if (ast->token)
  {
    token_free(ast->token);
  }

  if (ast->string_value)
  {
    free(ast->string_value);
  }

  if (ast->from_module)
  {
    free(ast->from_module);
  }

  if (ast->compiled_value)
  {
    free(ast->compiled_value);
  }

  if (ast->name)
  {
    free(ast->name);
  }

 if (ast->list_value)
 {
   if (ast->list_value->items)
   {
     free(ast->list_value->items);
   }

   free(ast->list_value);
 }

if (ast->flags)
 {
   if (ast->flags->items)
   {
     free(ast->flags->items);
   }

   free(ast->flags);
 }
  
  free(ast);
  ast = 0;
}
