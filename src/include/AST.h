#ifndef FJB_AST_H
#define FJB_AST_H
#include "token.h"
#include "list.h"
#include "gc.h"
typedef struct FJB_AST_STRUCT {
  enum {
    AST_ARROW_DEFINITION,
    AST_CALL,
    AST_FUNCTION,
    AST_ARRAY,
    AST_STRING,
    AST_INT,
    AST_HEX,
    AST_FLOAT,
    AST_ASSIGNMENT,
    AST_DEFINITION,
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

  unsigned int capsulated;
  unsigned int label;
  unsigned int lazy;
  unsigned int loose;
  unsigned int is_ref;
  unsigned int alive;
  unsigned int exported;
  unsigned int from_obj;
  unsigned int visited;
  int line;

} AST_T;


AST_T* init_ast(int type);

AST_T* init_ast_line(int type, int line);

AST_T* init_assignment(char* name, AST_T* value);

char* ast_to_str(AST_T* ast);

char* ast_binop_to_str(AST_T* ast);

void list_free(gc_T* gc, list_T* list);

void ast_free(AST_T* ast);
#endif
