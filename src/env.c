#include "include/env.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <stdlib.h>
#include <string.h>

fjb_env_T* FJB_ENV;

void init_fjb_env()
{
  FJB_ENV = calloc(1, sizeof(struct FJB_ENV_STRUCT));
  FJB_ENV->source = 0;
  FJB_ENV->filepath = 0;
  FJB_ENV->search_index = NEW_STACK;
  FJB_ENV->hooks = init_list(sizeof(plugin_hook));
  FJB_ENV->imports = NEW_MAP();
  FJB_ENV->functions = NEW_MAP();
  FJB_ENV->assignments = NEW_MAP();

  FJB_ENV->GC = init_gc();

  /* ==== Built-in globals ==== */
  AST_T* module = init_ast(AST_OBJECT);
  module->name = strdup("module");
  module->list_value = init_list(sizeof(AST_T*));
  gc_mark(FJB_ENV->GC, module);

  AST_T* exports = init_ast(AST_OBJECT);
  exports->list_value = init_list(sizeof(AST_T*));
  exports->name = strdup("exports");
  gc_mark(FJB_ENV->GC, exports);

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  assignment->name = strdup("module");
  assignment->value = module;
  gc_mark(FJB_ENV->GC, assignment);

  AST_T* exports_assignment = init_ast(AST_ASSIGNMENT);
  exports_assignment->name = strdup("exports");
  exports_assignment->value = exports;
  gc_mark(FJB_ENV->GC, exports_assignment);

  list_push(module->list_value, exports_assignment);

  FJB_ENV->module = module;
  FJB_ENV->exports = exports;
}

void destroy_fjb_env()
{
  gc_sweep(FJB_ENV->GC);
  gc_free(FJB_ENV->GC);
}

void fjb_set_aliased_import(unsigned int aliased_import)
{
  FJB_ENV->aliased_import = aliased_import;
}

void fjb_set_source(char* source)
{
  FJB_ENV->source = source ? strdup(source) : 0;
}

void fjb_set_filepath(char* filepath)
{
  FJB_ENV->filepath = filepath ? strdup(filepath) : 0;
}

list_T* fjb_get_hooks()
{
  return FJB_ENV->hooks;
}

void* fjb_call_all_hooks(int type, void* ptr, fjb_env_T* env)
{
  for (unsigned int i = 0; i < FJB_ENV->hooks->size; i++) {
    plugin_hook hook = (plugin_hook)FJB_ENV->hooks->items[i];
    if (!hook)
      continue;

    ptr = hook(type, ptr, env);
  }

  return ptr;
}
