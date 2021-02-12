#include "../include/hooks/before_import.h"
#include "../include/jsx.h"
#include "../include/plugin.h"
#include "../include/string_utils.h"
#include <scss/scss.h>
#include <stdio.h>
#include <string.h>

char* _hook_scss(char* source, char* filepath, fjb_env_T* env)
{
  if (!source || !filepath)
    return filepath;

  // rewrite filename to .css so that FJB later treats it as a regular .css
  char* new_filepath = strdup(strip_ext(filepath));
  new_filepath = str_append(&new_filepath, ".css");

  init_scss();

  char* out = scss(env->source, 0);

  fjb_set_source(out);

  return new_filepath;
}

void* fjb_hook_before_import(int action, void* ptr, fjb_env_T* env)
{
  if (action != HOOK_BEFORE_IMPORT)
    return ptr;
  if (!ptr)
    return ptr;

  char* ext = (char*)get_filename_ext((char*)ptr);

  if (strcmp(ext, ".scss") == 0) {
    return _hook_scss(env->source, (char*)ptr, env);
  }

  return ptr;
}
