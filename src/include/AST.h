#ifndef FJB_AST_H
#define FJB_AST_H
#include "gc.h"
#include "list.h"
#include "token.h"
typedef struct FJB_AST_STRUCT
{
  enum
  {
    AST_ARROW_DEFINITION,
    AST_CALL,
    AST_FUNCTION,
    AST_ARRAY,
    AST_STRING,
    AST_INT,
    AST_INT_MIN,
    AST_HEX,
    AST_FLOAT,
    AST_ASSIGNMENT,
    // AST_DEFINITION,
    AST_COLON_ASSIGNMENT,
    AST_WHILE,
    AST_DO,
    AST_FOR,
    AST_CONDITION,
    AST_TERNARY,
    AST_COMPOUND,
    AST_UNDEFINED,
    AST_IMPORT,
    AST_IMPORT_PHONY,
    AST_OBJECT,
    AST_SCOPE,
    AST_NAME,
    AST_STATE,
    AST_BINOP,
    AST_UNOP,
    AST_TRY,
    AST_INCREMENT,
    AST_DECREMENT,
    AST_REGEX,
    AST_SWITCH,
    AST_SIGNATURE,
    AST_LABEL,
    AST_NOOP,
    AST_TUPLE
  } type;

  struct FJB_AST_STRUCT* value;
  struct FJB_AST_STRUCT* body;
  struct FJB_AST_STRUCT* body2;
  struct FJB_AST_STRUCT* left;
  struct FJB_AST_STRUCT* right;
  struct FJB_AST_STRUCT* expr;
  struct FJB_AST_STRUCT* ref;
  struct FJB_AST_STRUCT* label_value;
  struct FJB_AST_STRUCT* phony_value;
  struct FJB_AST_STRUCT* ptr;
  struct FJB_AST_STRUCT* prev;
  struct FJB_AST_STRUCT* source_ast;
  struct FJB_AST_STRUCT* def;
  struct FJB_AST_STRUCT* parent;
  struct FJB_AST_STRUCT* next;
  struct FJB_AST_STRUCT* node;
  struct FJB_AST_STRUCT* condition;
  token_T* token;

  int int_value;
  float float_value;
  char* string_value;
  char* name;
  char* compiled_value;
  char* from_module;

  list_T* list_value;
  list_T* flags;
  list_T* es_exports;
  list_T* stack_frame;

  unsigned int capsulated;
  unsigned int lazy;
  unsigned int saved;
  unsigned int from_obj;
  int line;

  list_T* parent_lists;

} AST_T;

AST_T* init_ast(int type);

AST_T* init_ast_line(int type, int line);

AST_T* init_assignment(char* name, AST_T* value);

void ast_init_parent_lists(AST_T* ast);

unsigned int ast_is_in_list(AST_T* ast, list_T* list);

char* ast_to_str(AST_T* ast);

char* ast_binop_to_str(AST_T* ast);

list_T* ast_get_pointers(AST_T* ast);

AST_T* get_node_by_name(list_T* list, char* name);

typedef struct
{
  char* name;
  int type;
} AST_query_T;
AST_T* ast_query(list_T* list, AST_query_T query);

AST_T* ast_search_pointer(AST_T* ast, int type);

char* ast_encode_strings(list_T* strings);

void list_free(gc_T* gc, list_T* list);

void ast_free(AST_T* ast);

#define NEW_STACK init_list(sizeof(AST_T*))

#define LOOP_NODES(list, C, NAME, WHAT)                                                            \
  for (unsigned int C = 0; C < list->size; C++) {                                                  \
    AST_T* NAME = (AST_T*)list->items[C];                                                          \
    WHAT;                                                                                          \
  }

#define LOOP_NODES_FIXED(list, C, S, NAME, WHAT)                                                   \
  for (unsigned int C = 0; C < S; C++) {                                                           \
    AST_T* NAME = (AST_T*)list->items[C];                                                          \
    WHAT;                                                                                          \
  }

#endif
