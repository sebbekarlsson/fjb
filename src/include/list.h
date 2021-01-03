#ifndef FJB_LIST_H
#define FJB_LIST_H
#include <stdlib.h>

typedef struct LIST_STRUCT
{
  void** items;
  size_t size;
  size_t item_size;
} list_T;

list_T* init_list(size_t item_size);

void list_push(list_T* list, void* item);

void list_prefix(list_T* list, void* item);

unsigned int ptr_in_list(list_T* list, void* ptr);

int list_indexof_str(list_T* list, char* item);
#endif

