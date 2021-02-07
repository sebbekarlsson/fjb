#include "../include/hooks/source.h"
#include "../include/string_utils.h"
#include "../include/jsx.h"
#include <stdio.h>

void* fjb_hook_source(int action, void* ptr, fjb_env_T* env)
{
  if (action != HOOK_RECEIVE_SOURCE) return ptr;
  if (!ptr) return ptr;
  char* source = (char*) ptr;

  if (str_contains(source, "React"))
  {
    fjb_set_jsx_type(JSX_REACT);
  }

  return ptr;
}
