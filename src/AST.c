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

#define PAD(v) str_prefix(v, (const char*)get_indent(indent))

#define NL() str = str_append(&str, "\n")

#define SPACE() str = str_append(&str, " ")

#define LEFT()                                                                                     \
  char* _left = ast_to_str(ast->left);                                                             \
  str = str_append(&str, _left);

#define RIGHT()                                                                                    \
  char* _right = ast_to_str(ast->right);                                                           \
  str = str_append(&str, _right);

char* MKPAD(char* source, int indent)
{
  PAD(&source);
  return source;
}

char* ast_compound_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  indent += 1;
  LOOP_NODES(
    ast->list_value, i, child, char* childstr = ast_to_str(child); str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1) {
      char* comma = MKPAD(",", indent);
      str = str_append(&str, comma);
      NL();
    });
  indent -= 1;

  return str;
}

char* ast_tuple_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  char* lbracket = MKPAD("[\n", indent);
  str = str_append(&str, lbracket);

  indent += 1;
  LOOP_NODES(
    ast->list_value, i, child, char* childstr = ast_to_str(child); str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1) {
      char* comma = MKPAD(",", indent);
      str = str_append(&str, comma);
      NL();
    });
  indent -= 1;

  char* rbracket = MKPAD("]", indent);
  NL();
  str = str_append(&str, rbracket);

  return str;
}

char* ast_call_to_str(AST_T* ast, int indent)
{
  indent += 1;
  char* str = 0;

  {
    LEFT();
  }

  char* left = MKPAD("(\n", indent);
  str = str_append(&str, left);

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = ast_to_str(child); str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1) {
      char* comma = MKPAD(",", indent);
      str = str_append(&str, comma);
      NL();
    });
  indent -= 1;

  char* right = MKPAD(")", indent);
  NL();
  str = str_append(&str, right);

  return str;
}

char* ast_object_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  char* left = MKPAD("{\n", indent);
  str = str_append(&str, left);

  indent += 1;
  LOOP_NODES(
    ast->list_value, i, child, char* childstr = ast_to_str(child); str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1) {
      char* comma = MKPAD(",", indent);
      str = str_append(&str, comma);
      NL();
    });
  indent -= 1;

  char* right = MKPAD("}", indent);
  NL();
  str = str_append(&str, right);

  return str;
}

char* ast_function_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  char* lparen = MKPAD("(\n", indent);
  str = str_append(&str, lparen);

  indent += 1;

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = ast_to_str(child); str = str_append(&str, childstr);

    if (i < ast->list_value->size - 1) {
      char* comma = MKPAD(",", indent);
      str = str_append(&str, comma);
      NL();
    });

  indent -= 1;

  NL();
  char* rparen = strdup(")");
  PAD(&rparen);
  str = str_append(&str, rparen);

  NL();
  char* lbrace = MKPAD("{\n", indent);
  str = str_append(&str, lbrace);

  indent += 1;
  char* body = ast_to_str(ast->body);
  str = str_append(&str, body);
  indent -= 1;

  char* rbrace = MKPAD("}", indent);
  NL();
  str = str_append(&str, rbrace);

  return str;
}

char* ast_import_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  char* lbrace = MKPAD("{\n", indent);
  str = str_append(&str, lbrace);

  indent += 1;

  if (ast->compiled_value) {
    char buff[256];
    sprintf(buff, "BYTES(%ld)", (strlen(ast->compiled_value) + 1) * sizeof(char));

    char* body = strdup(buff);
    PAD(&body);
    str = str_append(&str, body);
  }
  indent -= 1;

  char* rbrace = MKPAD("}", indent);
  NL();
  str = str_append(&str, rbrace);

  return str;
}

char* ast_condition_to_str(AST_T* ast, int indent)
{
  char* str = 0;

  char* lparen = MKPAD("(\n", indent);
  str = str_append(&str, lparen);

  indent += 1;

  char* expr = ast_to_str(ast->expr);
  str = str_append(&str, expr);

  indent -= 1;

  NL();
  char* rparen = strdup(")");
  PAD(&rparen);
  str = str_append(&str, rparen);

  NL();
  char* lbrace = MKPAD("{\n", indent);
  str = str_append(&str, lbrace);

  indent += 1;
  char* body = ast_to_str(ast->body);
  str = str_append(&str, body);
  indent -= 1;

  char* rbrace = MKPAD("}", indent);
  NL();
  str = str_append(&str, rbrace);

  indent += 1;
  RIGHT();
  indent -= 1;

  return str;
}

char* ast_to_str_default(AST_T* ast, int indent)
{
  return strdup((AST_TYPE_STR[ast->type]));
}

char* ast_name_to_str(AST_T* ast, int indent)
{
  char* value = strdup(ast_get_string(ast));
  indent += 1;
  PAD(&value);
  indent -= 1;
  return value;
}

char* ast_state_to_str(AST_T* ast, int indent)
{
  indent += 1;
  char* name = ast_get_string(ast);
  name = str_append(&name, " ");
  PAD(&name);
  char* value = ast_to_str(ast->value);

  char* str = 0;
  str = str_append(&str, name);
  str = str_append(&str, value);
  indent -= 1;
  return value;
}

char* ast_assignment_to_str(AST_T* ast, int indent)
{
  indent += 1;
  char* str = 0;

  LEFT();

  char* value = ast_to_str(ast->value);

  if (ast->value) {
    str = str_append(&str, value);
  }
  indent -= 1;
  return str;
}

char* _ast_to_str(AST_T* ast, int indent)
{
  if (!ast)
    return 0;

  char* str = 0;

  char* head = ast_to_str_default(ast, indent);
  PAD(&head);
  str = str_append(&str, head);

  char buff[256];
  sprintf(buff, "%p", ast);
  str = str_append(&str, "[");
  str = str_append(&str, buff);
  str = str_append(&str, "]");

  str = str_append(&str, "(\n");

  switch (ast->type) {
    case AST_COMPOUND: str = str_append(&str, ast_compound_to_str(ast, indent)); break;
    case AST_IMPORT: str = str_append(&str, ast_import_to_str(ast, indent)); break;
    case AST_ARRAY:
    case AST_TUPLE: str = str_append(&str, ast_tuple_to_str(ast, indent)); break;
    case AST_CALL: str = str_append(&str, ast_call_to_str(ast, indent)); break;
    case AST_OBJECT: str = str_append(&str, ast_object_to_str(ast, indent)); break;
    case AST_FUNCTION: str = str_append(&str, ast_function_to_str(ast, indent)); break;
    case AST_WHILE:
    case AST_TRY:
    case AST_CONDITION: str = str_append(&str, ast_condition_to_str(ast, indent)); break;
    case AST_STRING:
    case AST_REGEX:
    case AST_INT:
    case AST_INT_MIN:
    case AST_FLOAT:
    case AST_NAME: str = str_append(&str, ast_name_to_str(ast, indent)); break;
    case AST_STATE: str = str_append(&str, ast_state_to_str(ast, indent)); break;
    case AST_COLON_ASSIGNMENT:
    case AST_ASSIGNMENT: str = str_append(&str, ast_assignment_to_str(ast, indent)); break;
    default: { /* noop */
    } break;
  }

  NL();
  char* rparen = strdup(")\n");
  PAD(&rparen);
  str = str_append(&str, rparen);

  char* right = ast_to_str(ast->right);
  str = str_append(&str, right);
  char* next = ast_to_str(ast->next);
  str = str_append(&str, next);

  return str;
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
