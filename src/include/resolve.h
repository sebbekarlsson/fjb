#ifndef FJB_RESOLVE_H
#define FJB_RESOLVE_H
#include "AST.h"

typedef struct QUERY_STRUCT_T
{
  char* name;
  int type;
  list_T* ignore;
  AST_T* parent;
} query_T;

AST_T* resolve(AST_T* ast, unsigned int (*query)(AST_T* ast, query_T data), query_T data);
#endif
