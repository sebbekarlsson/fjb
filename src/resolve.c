#include "include/resolve.h"
#include <string.h>

#define STACK() ast->stack_frame

#define MAYBE_RESOLVE(ast)                                                                         \
  if (ast && ast->name && strcmp(ast->name, name) == 0)                                                   \
  return ast

AST_T* resolve_array(AST_T* ast, char* name)
{
  AST_T* resolved = 0;

  LOOP_NODES(ast->list_value, i, child,
      resolved = resolve(child, name);
      if (resolved) return resolved;     
  );

  return resolved;
}
AST_T* resolve_hex(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_int(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_float(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_string(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_arrow_definition(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_colon_assignment(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);
  AST_T* V = resolve(ast->value, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;
  if (V)
    return V;

  return 0;
}
AST_T* resolve_while(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);
  AST_T* V = resolve(ast->value, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;
  if (V)
    return V;

  return 0;
}
AST_T* resolve_for(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);
  AST_T* V = resolve(ast->value, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;
  if (V)
    return V;

  return 0;
}
AST_T* resolve_compound(AST_T* ast, char* name)
{
  AST_T* resolved = 0;

  LOOP_NODES(ast->list_value, i, child,
      resolved = resolve(child, name);
      if (resolved) return resolved;     
  );

  return resolved;
}
static AST_T* resolve_import(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_undefined(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_call(AST_T* ast, char* name)
{
  MAYBE_RESOLVE(ast);
  MAYBE_RESOLVE(ast->left);

  AST_T* resolved = 0;

  LOOP_NODES(ast->list_value, i, child,
      resolved = resolve(child, name);
      if (resolved) return resolved;     
  );

  return resolved;
}
AST_T* resolve_function(AST_T* ast, char* name)
{
  MAYBE_RESOLVE(ast);
  return resolve(ast->body, name);
}
AST_T* resolve_signature(AST_T* ast, char* name)
{
  MAYBE_RESOLVE(ast);

  return 0;
}
AST_T* resolve_name(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_state(AST_T* ast, char* name)
{
  return resolve(ast->value, name);
}
AST_T* resolve_binop(AST_T* ast, char* name)
{
  MAYBE_RESOLVE(ast->left);
  MAYBE_RESOLVE(ast->right);

  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_unop(AST_T* ast, char* name)
{
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_increment(AST_T* ast, char* name)
{
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_decrement(AST_T* ast, char* name)
{
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_regex(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_object(AST_T* ast, char* name)
{
  AST_T* resolved = 0;

  LOOP_NODES(ast->list_value, i, child,
      resolved = resolve(child, name);
      if (resolved) return resolved;     
  );

  return resolved;
}
AST_T* resolve_condition(AST_T* ast, char* name)
{

  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_switch(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_try(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_ternary(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);
  AST_T* V = resolve(ast->value, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;
  if (V)
    return V;

  return 0;
}
AST_T* resolve_do(AST_T* ast, char* name)
{
  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;

  return 0;
}
AST_T* resolve_noop(AST_T* ast, char* name)
{
  return 0;
}
AST_T* resolve_tuple(AST_T* ast, char* name)
{
  AST_T* resolved = 0;

  LOOP_NODES(ast->list_value, i, child,
      resolved = resolve(child, name);
      if (resolved) return resolved;     
  );

  return resolved;
}
AST_T* resolve_assignment(AST_T* ast, char* name)
{
  MAYBE_RESOLVE(ast);
  MAYBE_RESOLVE(ast->left);

  AST_T* A = resolve(ast->expr, name);
  AST_T* B = resolve(ast->body, name);
  AST_T* L = resolve(ast->left, name);
  AST_T* R = resolve(ast->right, name);
  AST_T* V = resolve(ast->value, name);

  if (A)
    return A;
  if (B)
    return B;
  if (L)
    return L;
  if (R)
    return R;
  if (V)
    return V;

  return 0;
}

AST_T* resolve(AST_T* ast, char* name)
{
  if (!ast) return 0;
  
  MAYBE_RESOLVE(ast->next);

  switch (ast->type) {
    case AST_ARRAY: ast = resolve_array(ast, name); break;
    case AST_HEX: ast = resolve_hex(ast, name); break;
    case AST_INT: ast = resolve_int(ast, name); break;
    case AST_FLOAT: ast = resolve_float(ast, name); break;
    case AST_STRING: ast = resolve_string(ast, name); break;
    case AST_ARROW_DEFINITION: ast = resolve_arrow_definition(ast, name); break;
    case AST_ASSIGNMENT: ast = resolve_assignment(ast, name); break;
    case AST_COLON_ASSIGNMENT: ast = resolve_colon_assignment(ast, name); break;
    case AST_WHILE: ast = resolve_while(ast, name); break;
    case AST_FOR: ast = resolve_for(ast, name); break;
    case AST_COMPOUND: ast = resolve_compound(ast, name); break;
    case AST_IMPORT: ast = resolve_import(ast, name); break;
    case AST_UNDEFINED: ast = resolve_undefined(ast, name); break;
    case AST_CALL: ast = resolve_call(ast, name); break;
    case AST_FUNCTION: ast = resolve_function(ast, name); break;
    case AST_SIGNATURE: ast = resolve_signature(ast, name); break;
    case AST_NAME: ast = resolve_name(ast, name); break;
    case AST_STATE: ast = resolve_state(ast, name); break;
    case AST_BINOP: ast = resolve_binop(ast, name); break;
    case AST_UNOP: ast = resolve_unop(ast, name); break;
    case AST_INCREMENT: ast = resolve_increment(ast, name); break;
    case AST_DECREMENT: ast = resolve_decrement(ast, name); break;
    case AST_REGEX: ast = resolve_regex(ast, name); break;
    case AST_OBJECT: ast = resolve_object(ast, name); break;
    case AST_CONDITION: ast = resolve_condition(ast, name); break;
    case AST_SWITCH: ast = resolve_switch(ast, name); break;
    case AST_TRY: ast = resolve_try(ast, name); break;
    case AST_TERNARY: ast = resolve_ternary(ast, name); break;
    case AST_DO: ast = resolve_do(ast, name); break;
    case AST_NOOP: ast = resolve_noop(ast, name); break;
    case AST_TUPLE: ast = resolve_tuple(ast, name); break;
    default: {
      MAYBE_RESOLVE(ast);
    } break;
  }

  return ast;
}
