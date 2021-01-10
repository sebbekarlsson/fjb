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
  ast->line = 0;

  return ast;
}

AST_T* init_ast_line(int type, int line)
{
  AST_T* ast = init_ast(type);
  ast->line = line;
}

char* ast_binop_to_str(AST_T* ast)
{
  char* left = ast_to_str(ast->left);
  char* tok = token_to_str(ast->token);
  char* right = ast_to_str(ast->right);

  left = left ? left : strdup("");
  tok = tok ? tok : strdup("");
  right = right ? right : strdup("");

  const char* template = "BINOP(\n%s ->\n%s ->\n%s\n)";
  char* value =
    calloc(strlen(template) + strlen(left) + strlen(tok) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, tok, right);

  free(left);
  free(tok);
  free(right);

  return value;
}

char* ast_unop_to_str(AST_T* ast)
{
  char* left = ast->left ? ast_to_str(ast->left) : strdup("");
  char* tok = token_to_str(ast->token);
  char* right = ast->right ? ast_to_str(ast->right) : strdup("");

  const char* template = "UNOP(%s -> %s -> %s)";
  char* value =
    calloc(strlen(template) + strlen(left) + strlen(tok) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, tok, right);

  if (left)
    free(left);

  if (tok)
    free(tok);

  if (right)
    free(right);

  return value;
}

char* ast_name_to_str(AST_T* ast)
{
  const char* template = "NAME(%s)";
  char* v = ast->name ? ast->name : ast->string_value ? ast->string_value : "";
  char* value = calloc(strlen(template) + strlen(v) + 1, sizeof(char));
  sprintf(value, template, v);

  return value;
}

char* ast_state_to_str(AST_T* ast)
{
  const char* template = "STATE(%s -> %s -> %s)";
  char* exprstr = ast->expr ? ast_to_str(ast->expr) : "";
  char* rightstr = ast->right ? ast_to_str(ast->right) : ast->value ? ast_to_str(ast->value) : "";
  char* v = ast->name ? ast->name : ast->string_value ? ast->string_value : "";
  char* value = calloc(strlen(template) + strlen(exprstr) + strlen(rightstr) + strlen(v) + 1, sizeof(char));
  sprintf(value, template, v, exprstr, rightstr);

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
  const char* template = "%s.CALL(%s)";
  char* name = ast->left ? ast_to_str(ast->left) : "UNKNOWN";

  char* str = 0;

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* child = ast->list_value->items[i];

    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1)
      str = str_append(&str, ",\n");
  }

  str = str ? str : "";
  
  char* value = calloc(strlen(template) + strlen(str) + strlen(name) + 128, sizeof(char));

  sprintf(value, template, name, str);

  return value;
}

char* ast_assignment_to_str(AST_T* ast)
{
  const char* template = "ASSIGNMENT(%s = %s)";

  char* name = ast->left ? ast_to_str(ast->left) : ast->name ? ast->name : "UNKNOWN";
  
  char* val = ast->value ? ast_to_str(ast->value) : "(nil)";
  char* value = calloc(strlen(template) + (name ? strlen(name) : 0) + strlen(val) + 1, sizeof(char));
  sprintf(value, template, name, val ? val : "");

  return value;
}

char* ast_definition_to_str(AST_T* ast)
{
  char* val = 0;

  if (ast->expr)
  {
    char* exprstr = ast_to_str(ast->expr);
    val = str_append(&val, exprstr);
  }
  
  if (ast->list_value)
  {
    for (unsigned int i = 0; i < ast->list_value->size; i++)
    {
      AST_T* child = ast->list_value->items[i];

      char* childstr = ast_to_str(child);
      val = str_append(&val, childstr);
    }
  }

  return val ? val : strdup("(nil)");
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
      str = str_append(&str, ",\n");
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

  char* name = ast->name ? ast->name : "";

  char* value = calloc(strlen(template) + strlen(str) + strlen(name) + 1, sizeof(char));
  sprintf(value, template, name, str);

  if (ast->body) {
    char* bodystr = ast_to_str(ast->body);

    if (bodystr)
      value = str_append(&value, bodystr);
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

    if (!childstr) continue;

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

  //if (ast->ptr)
  //  return ast_to_str(ast->ptr);

  switch (ast->type) {
    case AST_BINOP: return ast_binop_to_str(ast); break;
    case AST_UNOP: return ast_unop_to_str(ast); break;
    case AST_NAME: return ast_name_to_str(ast); break;
    case AST_STATE: return ast_state_to_str(ast); break;
    case AST_INT: return ast_int_to_str(ast); break;
    case AST_ARROW_DEFINITION: return ast_arrow_definition_to_str(ast); break;
    case AST_CALL: return ast_call_to_str(ast); break;
    case AST_OBJECT: return ast_object_to_str(ast); break;
    case AST_ASSIGNMENT: return ast_assignment_to_str(ast); break;
    case AST_DEFINITION: return ast_definition_to_str(ast); break;
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

if (ast->es_exports)
 {
   if (ast->es_exports->items)
   {
     free(ast->es_exports->items);
   }

   free(ast->es_exports);
 }
  
  free(ast);
  ast = 0;
}

AST_T* init_assignment(char* name, AST_T* value)
{
  AST_T* ast = init_ast(AST_ASSIGNMENT);
  ast->name = strdup(name);
  ast->left = init_ast(AST_NAME);
  ast->left->name = strdup(name);
  ast->value = value;

  return ast;
}
