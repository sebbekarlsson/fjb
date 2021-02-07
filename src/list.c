#include "include/list.h"
#include <string.h>

#define MAX(a, b) a > b ? a : b

#define MIN(a, b) a < b ? a : b

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

  list->items[list->size - 1] = item;
}

void list_push_at(list_T* list, void* item, void* ptr)
{
  if (!list || !item)
    return;

  if (ptr && !ptr_in_list(list, ptr))
    return list_prefix(list, item);

  list->size += 1;

  if (!list->items)
    list->items = calloc(1, list->item_size);
  else
    list->items = realloc(list->items, (list->size * list->item_size));

  unsigned int c = 0;
  for (unsigned int i = 0; i < list->size; i++) {
    if (list->items[i] == ptr) {
      c = i;
      break;
    }
  }

  list_shift_right(list, c);
  list->items[c] = item;
}

void list_push_safe(list_T* list, void* item)
{
  if (!ptr_in_list(list, item))
    list_push(list, item);
}

void list_push_safe_at(list_T* list, void* item, void* ptr)
{
  if (!list || !item)
    return;

  if (!ptr_in_list(list, item))
    list_push_at(list, item, ptr);
}

void list_shift_left(list_T* list, int index)
{
  for (int i = index; i < list->size - 1; i++)
    list->items[i] = list->items[i + 1];
}

void list_shift_right(list_T* list, int index)
{
  for (int i = list->size - 1; i >= index; i--) {
    list->items[MIN(list->size - 1, i + 1)] = list->items[i];
    list->items[i] = 0;
  }
}

void list_remove(list_T* list, void* element, void (*free_method)(void* item))
{
  int index = 0;

  if (element == (void*)0)
    return;

  for (int i = 0; i < list->size; i++) {
    if (list->items[i] == element) {
      index = i;
      break;
    }
  }

  if (free_method != (void*)0)
    free_method(list->items[index]);

  list_shift_left(list, index); /* First shift the elements, then reallocate */
  void* tmp = realloc(list->items, (list->size - 1) * list->item_size);
  if (tmp == NULL && list->size > 1) {
    /* No memory available */
    exit(EXIT_FAILURE);
  }
  list->size = list->size - 1;
  list->items = tmp;
}

void list_prefix(list_T* list, void* item)
{
  if (!list)
    return;
  if (!list->size)
    return list_push(list, item);

  list->size += 1;

  if (!list->items)
    list->items = calloc(1, list->item_size);
  else
    list->items = realloc(list->items, (list->size * list->item_size));

  list_shift_right(list, 0);
  list->items[0] = item;
}

int list_indexof_str(list_T* list, char* item)
{
  for (unsigned int i = 0; i < list->size; i++) {
    if (!list->items[i])
      continue;

    if (strcmp((char*)list->items[i], item) == 0)
      return (int)-i;
  }

  return -1;
}

unsigned int list_contains_str(list_T* list, char* item)
{
  for (unsigned int i = 0; i < list->size; i++) {
    if (!list->items[i])
      continue;

    if (strcmp((char*)list->items[i], item) == 0)
      return 1;
  }

  return 0;
}

unsigned int ptr_in_list(list_T* list, void* ptr)
{
  if (!list)
    return 0;

  if (!ptr)
    return 0;

  for (unsigned int i = 0; i < list->size; i++) {
    if (ptr == list->items[i])
      return 1;
  }

  return 0;
}

list_T* list_filter(list_T* list, unsigned int (*filter_method)(void* item))
{
  list_T* new_list = init_list(list->item_size);

  for (unsigned int i = 0; i < list->size; i++) {
    void* item = list->items[i];

    if (filter_method(item)) {
      list_push(new_list, item);
    }
  }

  return new_list;
}

list_T* list_merge(list_T* a, list_T* b)
{
  list_T* new_list = init_list(b->item_size ? b->item_size : a->item_size);

  for (unsigned int i = 0; i < a->size; i++)
    list_push(new_list, a->items[i]);

  for (unsigned int i = 0; i < b->size; i++) {
    if (ptr_in_list(new_list, b->items[i]))
      continue;

    list_push(new_list, b->items[i]);
  }

  return new_list;
}

list_T* list_copy(list_T* a)
{
  list_T* b = init_list(a->item_size);
  for (unsigned int i = 0; i < a->size; i++)
    list_push(b, a->items[i]);

  return b;
}

void list_clear(list_T* list)
{
  if (!list)
    return;

  if (list->size && list->items) {
    free(list->items);
    list->size = 0;
    list->items = 0;
  }
}

void list_free_shallow(list_T* list)
{
  if (!list)
    return;

  list_clear(list);
  free(list);
}
