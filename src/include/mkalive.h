#ifndef FJB_MKALIVE_H
#define FJB_MKALIVE_H
#include "visitor.h"
#include "AST.h"

#define ALIVE(x)\
  mkalive(visitor, x, args, stack);

#define ALIVE_LIST(list)\
  mkalive_list(visitor, list, args, stack);

AST_T* mkalive(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack);

list_T* mkalive_list(visitor_T* visitor, list_T* list, list_T* args, list_T* stack);
#endif
