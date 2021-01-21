#ifndef FJB_COMPOUND_H
#define FJB_COMPOUND_H
#include "AST.h"
#include "list.h"
AST_T* new_compound(AST_T* lookup, list_T* imports, list_T* es_exports);
#endif
