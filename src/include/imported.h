#ifndef FJB_IMPORTED_H
#define FJB_IMPORTED_H
#include "AST.h"

typedef struct IMPORTED
{
  char* name;
  char* alias;
  AST_T* ast;
} imported_T;

imported_T* init_imported(char* name, char* alias, AST_T* ast);

#endif
