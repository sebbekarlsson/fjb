#include "include/gc.h"
#include "include/AST.h"
#include <list/list.h>
#include <stdio.h>

gc_T* init_gc()
{
  gc_T* gc = calloc(1, sizeof(struct FJB_GC_STRUCT));
  gc->trash = init_list(sizeof(AST_T*));
  gc->trashy_lists = init_list(sizeof(list_T*));

  return gc;
}

void gc_mark(gc_T* gc, AST_T* item)
{
  if (!item)
    return;

  list_push(gc->trash, item);
}

void gc_mark_list(gc_T* gc, list_T* list)
{
  if (!list)
    return;

  // if (ptr_in_list(gc->trashy_lists, list))
  //  return;

  list_push(gc->trashy_lists, list);
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

  for (unsigned int i = 0; i < gc->trashy_lists->size; i++) {
    list_T* child = gc->trashy_lists->items[i];

    list_free_shallow(child);
  }

  free(gc->trashy_lists->items);
  free(gc->trashy_lists);
  gc->trashy_lists = init_list(sizeof(AST_T*));
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

  list_free_shallow(gc->trashy_lists);

  free(gc);
}
