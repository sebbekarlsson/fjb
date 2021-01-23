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

char* ast_binop_to_str(AST_T* ast)
{
  char* left = ast_to_str(ast->left);
  char* tok = token_to_str(ast->token);
  char* right = ast_to_str(ast->right);

  left = left ? left : strdup("");
  tok = tok ? tok : strdup("");
  right = right ? right : strdup("");

  const char* template = "BINOP(%s -> %s -> %s)";
  char* value =
    calloc(strlen(template) + strlen(left) + strlen(tok) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, tok, right);

  free(left);
  free(tok);
  free(right);

  return value;
}

char* ast_ternary_to_str(AST_T* ast)
{
  char* left = ast_to_str(ast->left);
  char* val = ast_to_str(ast->value);
  char* right = ast_to_str(ast->right);

  left = left ? left : strdup("");
  val = val ? val : strdup("");
  right = right ? right : strdup("");

  const char* template = "TERNARY(%s ? %s : %s)";
  char* value =
    calloc(strlen(template) + strlen(left) + strlen(val) + strlen(right) + 1, sizeof(char));
  sprintf(value, template, left, val, right);

  free(left);
  free(val);
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
  char* value =
    calloc(strlen(template) + strlen(exprstr) + strlen(rightstr) + strlen(v) + 1, sizeof(char));
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
      str = str_append(&str, ",");
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
  char* value =
    calloc(strlen(template) + (name ? strlen(name) : 0) + strlen(val) + 1, sizeof(char));
  sprintf(value, template, name, val ? val : "");

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

    if (!childstr)
      continue;

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

char* ast_noop_to_str(AST_T* ast)
{
  return strdup("NOOP()");
}

char* ast_undefined_to_str(AST_T* ast)
{
  return strdup("UNDEFINED()");
}

char* ast_to_str(AST_T* ast)
{
  if (!ast)
    return strdup("AST(nil)");

  switch (ast->type) {
    case AST_BINOP: return ast_binop_to_str(ast); break;
    case AST_TERNARY: return ast_ternary_to_str(ast); break;
    case AST_UNOP: return ast_unop_to_str(ast); break;
    case AST_NAME: return ast_name_to_str(ast); break;
    case AST_NOOP: return ast_noop_to_str(ast); break;
    case AST_UNDEFINED: return ast_undefined_to_str(ast); break;
    case AST_STATE: return ast_state_to_str(ast); break;
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
  ast->name = strdup(name);
  ast->left = init_ast(AST_NAME);
  ast->left->name = strdup(name);
  ast->value = value;

  return ast;
}

list_T* ast_get_pointers(AST_T* ast)
{
  list_T* list = init_list(sizeof(AST_T*));

  if (!ast)
    return list;

  AST_T* ptr = ast->ptr;

  while (ptr && !ptr_in_list(list, ptr)) {
    if (ptr) {
      list_push(list, ptr);

      if (ptr)
        ptr = ptr->ptr;
    }
  }

  return list;
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

AST_T* ast_search_pointer(AST_T* ast, int type)
{
  if (ast->type == type)
    return ast;
  list_T* pointers = ast_get_pointers(ast);

  AST_T* ptr = 0;

  for (unsigned int i = 0; i < pointers->size; i++) {
    AST_T* child = pointers->items[i];
    if (!child)
      continue;

    if (child->type == type) {
      ptr = child;
      break;
    }
  }

  if (pointers) {
    if (pointers->items)
      free(pointers->items);
    free(pointers);
  }

  return ptr;
}

AST_T* get_node_by_name(list_T* list, char* name)
{
  LOOP_NODES(list, i, child, {
    if (!child->name)
      continue;

    if (strcmp(child->name, name) == 0)
      return child;
  });

  return 0;
}

AST_T* ast_query(list_T* list, unsigned int (*match)(AST_T* ast, query_T query), query_T query)
{
  LOOP_NODES(list, i, child, {
      if (match(child, query)) return child;
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
  if (!ast->name && !ast->string_value) return 0;

  if (ast->string_value) return ast->string_value;
  if (ast->name) return ast->name;
  return 0;
}
