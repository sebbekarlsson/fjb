#include "include/eval.h"
#include "include/string_utils.h"
#include <string.h>

AST_T* eval(visitor_T* visitor, AST_T* ast)
{
  switch (ast->type) {
    case AST_BINOP: return eval_binop(visitor, ast); break;
    case AST_NAME: return eval_name(visitor, ast); break;
    default: return 0; break;
  }

  return 0;
}

AST_T* eval_name(visitor_T* visitor, AST_T* ast)
{
  if (!ast->from_obj && ast->name) {
    if (strcmp(ast->name, "false") == 0) {
      AST_T* new_name = init_ast(AST_INT);
      new_name->int_value = 0;
      return new_name;
    } else if (strcmp(ast->name, "true") == 0) {
      AST_T* new_name = init_ast(AST_INT);
      new_name->int_value = 1;
      return new_name;
    }
  }

  return ast;
}

static AST_T* _eval_string(char* leftstr, char* rightstr, int token_type)
{
  AST_T* ast = init_ast(AST_BOOL);

  if (leftstr == 0 && rightstr == 0) {
    ast->bool_value = 1;
    return ast;
  }

  if (!leftstr || !rightstr)
    return ast;

  switch (token_type) {
    case TOKEN_EQUALS_EQUALS:
    case TOKEN_EQUALS_EQUALS_EQUALS: ast->bool_value = strcmp(leftstr, rightstr) == 0; break;
    case TOKEN_NOT_EQUALS:
    case TOKEN_NOT_EQUALS_EQUALS: ast->bool_value = strcmp(leftstr, rightstr) != 0; break;
    case TOKEN_PLUS: {
      char* str = 0;
      str_append(&str, leftstr);
      str_append(&str, rightstr);
      ast->string_value = str;

      if (str)
        ast->type = AST_STRING;
    } break;
    default: ast->type = AST_NOOP; break;
  }

  return ast;
}

static AST_T* _eval_number(int left, int right, int token_type)
{
  AST_T* ast = init_ast(AST_INT);

  switch (token_type) {
    case TOKEN_EQUALS_EQUALS:
    case TOKEN_EQUALS_EQUALS_EQUALS: ast->int_value = (left == right); break;
    case TOKEN_NOT_EQUALS:
    case TOKEN_NOT_EQUALS_EQUALS: ast->int_value = (left != right); break;
    case TOKEN_PLUS: ast->int_value = (left + right); break;
    case TOKEN_MINUS: ast->int_value = (left - right); break;
    case TOKEN_DIV: {
      if (left != 0 && right != 0) {
        ast->int_value = (left / right);
      }
    } break;
    default: ast->type = AST_NOOP; break;
  }

  return ast;
}

AST_T* eval_binop(visitor_T* visitor, AST_T* ast)
{
  if (!ast->left || !ast->right || !ast->token)
    return 0;

  AST_T* res = 0;

  // strings
  if (ast->left->ptr && ast->left->ptr->type == AST_STRING && ast->right->type == AST_STRING) {
    res = _eval_string(ast->left->ptr->string_value, ast->right->string_value, ast->token->type);
  } else if (ast->left->type == AST_STRING && ast->right->type == AST_STRING) {
    res = _eval_string(ast->left->string_value, ast->right->string_value, ast->token->type);
  }

  // integers
  else if (ast->left->type == AST_INT && ast->right->type == AST_INT) {
    res = _eval_number(ast->left->int_value, ast->right->int_value, ast->token->type);
  }

  // integers
  else if (ast->left->type == AST_INT_MIN && ast->right->type == AST_INT_MIN) {
    res = _eval_number(ast->left->int_value, ast->right->int_value, ast->token->type);
  }

  // floats
  else if (ast->left->type == AST_FLOAT && ast->right->type == AST_FLOAT) {
    res = _eval_number(ast->left->int_value, ast->right->int_value, ast->token->type);
  }

  if (!res)
    return 0;
  return res->type == AST_NOOP ? 0 : res;
}
