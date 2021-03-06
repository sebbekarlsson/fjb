#ifndef FJB_AST_H
#define FJB_AST_H
#include "enums/AST_TYPES.gpp.h"
#include "gc.h"
#include "list.h"
#include "token.h"
#include <hashmap/map.h>
typedef struct FJB_AST_STRUCT
{
  enum AST_TYPE type;

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
  struct FJB_AST_STRUCT* ast;
  struct FJB_AST_STRUCT* condition;
  struct FJB_AST_STRUCT* name_ast;
  struct FJB_AST_STRUCT* typedata;
  token_T* token;

  int int_value;
  float float_value;
  char* string_value;
  char* alias;
  char* name;
  char* compiled_value;
  char* from_module;
  char* innerText;
  char* headers;
  char* encoding;

  list_T* list_value;
  list_T* options;
  list_T* flags;
  map_T* stack_frame;

  unsigned int capsulated;
  unsigned int lazy;
  unsigned int saved;
  unsigned int from_obj;
  unsigned int jsx_closed;
  unsigned int anon;
  unsigned int exported;
  unsigned int not_exported;
  unsigned int writable;
  unsigned int bool_value;
  unsigned int dead;
  unsigned int is_require_call;
  int line;

  unsigned int is_resolved;

  list_T* parent_lists;
  map_T* requirements;

  char* basename;
  char* comment;
  int flag_type;

  map_T* map;

} AST_T;

AST_T* init_ast(int type);

AST_T* init_ast_line(int type, int line);

AST_T* init_assignment(char* name, AST_T* value);

AST_T* init_ast_string(char* string_value);

AST_T* init_ast_name(char* name);

char* ast_type_to_str(AST_T* ast);

typedef struct QUERY_STRUCT_T
{
  char* name;
  int type;
  list_T* ignore;
  AST_T* parent;
} query_T;

AST_T* ast_search_pointer(AST_T* ast, int type);

char* ast_get_string(AST_T* ast);

char* ast_get_string_copy(AST_T* ast);

void list_free(gc_T* gc, list_T* list);

void ast_free(AST_T* ast);

list_T* ast_get_parents(AST_T* ast);

#define NEW_STACK init_list(sizeof(AST_T*))

#define LOOP_NODES(list, C, NAME, WHAT)                                                            \
  if (list && list->items && list->size) {                                                         \
    for (unsigned int C = 0; C < list->size; C++) {                                                \
      AST_T* NAME = (AST_T*)list->items[C];                                                        \
      WHAT;                                                                                        \
    }                                                                                              \
  }

#define LOOP_NODES_FIXED(list, C, S, NAME, WHAT)                                                   \
  if (list && list->items && list->size) {                                                         \
    for (unsigned int C = 0; C < S; C++) {                                                         \
      AST_T* NAME = (AST_T*)list->items[C];                                                        \
      WHAT;                                                                                        \
    }                                                                                              \
  }

#endif
