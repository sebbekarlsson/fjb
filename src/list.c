#include "include/list.h"
#include <string.h>

#define MAX(a, b)\
  a > b ? a : b

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

void list_push_safe(list_T* list, void* item)
{
  if (!ptr_in_list(list, item))
    list_push(list, item);
}

void list_shift_left(list_T* list, int index)
{
   for (int i = index; i < list->size - 1; i++)
       list->items[i] = list->items[i + 1];
}

void list_remove(list_T* list, void* element, void (*free_method)(void* item))
{
    int index = 0;

    if (element == (void*)0)
        return;

    for (int i = 0; i < list->size; i++)
    {
        if (list->items[i] == element)
        {
            index = i;
            break;
        }
    }

    if (free_method != (void*)0)    
        free_method(list->items[index]);

    list_shift_left(list, index);  /* First shift the elements, then reallocate */
    void *tmp = realloc(
        list->items, (list->size - 1) * list->item_size
    );
    if (tmp == NULL && list->size > 1) {
       /* No memory available */
       exit(EXIT_FAILURE);
    }
    list->size = list->size - 1;
    list->items = tmp;
}

void list_prefix(list_T* list, void* item)
{
  if (!list) return;

  list->size += 1;

  if (!list->items)
    list->items = calloc(1, list->item_size);
  else
    list->items = realloc(list->items, (list->size * list->item_size));

  for (unsigned int i = 0; i < list->size-1; i++)
  {
    list->items[i+1] = list->items[i];
  }
  list->items[0] = item;
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

unsigned int ptr_in_list(list_T* list, void* ptr)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if(ptr == list->items[i])
      return 1;
  }

  return 0;
}

list_T* list_filter(list_T* list, unsigned int (*filter_method)(void* item))
{
  list_T* new_list = init_list(list->item_size);

  for (unsigned int i = 0; i < list->size; i++)
  {
    void* item = list->items[i];

    if (filter_method(item))
    {
      list_push(new_list, item);
    }
  }

  return new_list;
}
