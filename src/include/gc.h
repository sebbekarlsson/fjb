#ifndef FJB_GC_H
#define FJB_GC_H
#include "list.h"

typedef struct FJB_AST_STRUCT AST_T;

typedef struct FJB_GC_STRUCT
{
  list_T* trash;
} gc_T;

gc_T* init_gc();
void gc_mark(gc_T* gc, AST_T* item);

void gc_sweep(gc_T* gc);

void gc_free(gc_T* gc);
#endif
