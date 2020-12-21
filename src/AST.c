#include "include/AST.h"
#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/string_utils.h"


AST_T* init_ast(int type)
{
  AST_T* ast = calloc(1, sizeof(struct FJB_AST_STRUCT));
  ast->type = type;

  return ast;
}

char* ast_binop_to_str(AST_T* ast)
{
  char* left = ast_to_str(ast->left);
  char* tok = token_to_str(ast->token);
  char* right = ast_to_str(ast->right);

  const char* template = "BINOP(%s -> %s -> %s)";
  char* value = calloc(strlen(template) + strlen(left) + strlen(tok) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, tok, right);

  return value;
}

char* ast_name_to_str(AST_T* ast)
{
  const char* template = "NAME(%s)";
  char* value = calloc(strlen(template) + strlen(ast->name) + 1, sizeof(char));
  sprintf(value, template, ast->name);

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
  const char* template = "%s.CALL";
  char* name = ast->left ? ast_to_str(ast->left) : "UNKNOWN";
  char* value = calloc(strlen(template) + strlen(name) + 1, sizeof(char));
  sprintf(value, template, name); 

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

  for (unsigned int i = 0; i < ast->list_value->size; i++)
  {
    AST_T* child = ast->list_value->items[i];
    char* childstr = ast_to_str(child);

    str = str_append(&str, childstr);

    if (i < ast->list_value->size-1)
      str = str_append(&str, ",");
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
  sprintf(value, template, ast ? (int) ast->type : 0);

  return value;
}

char* ast_to_str(AST_T* ast)
{
  if (!ast) return strdup("AST(nil)");

  switch (ast->type)
  {
    case AST_BINOP: return ast_binop_to_str(ast); break;
    case AST_NAME: return ast_name_to_str(ast); break;
    case AST_INT: return ast_int_to_str(ast); break;
    case AST_ARROW_DEFINITION: return ast_arrow_definition_to_str(ast); break;
    case AST_CALL: return ast_call_to_str(ast); break;
    case AST_OBJECT: return ast_object_to_str(ast); break;
    case AST_ASSIGNMENT: return ast_assignment_to_str(ast); break;
    default: return unstringable(ast); break;
  }
}
