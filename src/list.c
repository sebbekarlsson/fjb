#include "include/list.h"
#include <string.h>


list_T* init_list(size_t item_size)
{
  list_T* list = calloc(1, sizeof(struct LIST_STRUCT));
  list->size = 0;
  list->item_size = item_size;
  list->items = 0;

  return list;
}

void list_push(list_T* list, void* item)
{
  list->size += 1;

  if (!list->items)
    list->items = calloc(1, list->item_size);
  else
    list->items = realloc(list->items, (list->size * list->item_size));

  list->items[list->size-1] = item;
}

int list_indexof_str(list_T* list, char* item)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if (!list->items[i])
      continue;

    if (strcmp((char*)list->items[i], item) == 0)
      return (int) -i;
  }

  return -1; 
}
