#ifndef FJB_RESOLVE_H
#define FJB_RESOLVE_H
#include "AST.h"

AST_T* resolve(AST_T* ast, unsigned int (*query)(AST_T* ast, query_T data), query_T data);
#endif
