#include "include/gc.h"
#include "include/AST.h"
#include <stdio.h>

gc_T* init_gc()
{
  gc_T* gc = calloc(1, sizeof(struct FJB_GC_STRUCT));
  gc->trash = init_list(sizeof(AST_T*));

  return gc;
}

void gc_mark(gc_T* gc, AST_T* item)
{
  if (!item)
    return;

  list_push(gc->trash, item);
}

void gc_sweep(gc_T* gc)
{
  for (unsigned int i = 0; i < gc->trash->size; i++) {
    AST_T* child = gc->trash->items[i];

    ast_free(child);
  }

  free(gc->trash->items);
  free(gc->trash);
  gc->trash = init_list(sizeof(AST_T*));
}

void gc_free(gc_T* gc)
{
  gc_sweep(gc);
  if (gc->trash) {
    if (gc->trash->items) {
      free(gc->trash->items);
    }

    free(gc->trash);
  }

  free(gc);
}
