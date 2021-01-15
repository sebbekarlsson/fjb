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

void list_push_safe(list_T* list, void* item);

void list_shift_left(list_T* list, int index);

void list_remove(list_T* list, void* element, void (*free_method)(void* item));

void list_prefix(list_T* list, void* item);

unsigned int ptr_in_list(list_T* list, void* ptr);

int list_indexof_str(list_T* list, char* item);

list_T* list_filter(list_T* list, unsigned int (*filter_method)(void* item));
#endif

