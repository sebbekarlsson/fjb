#include "include/env.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

fjb_env_T* FJB_ENV;

void register_process_object()
{
  AST_T* ast = init_ast(AST_OBJECT);
  ast->name = strdup("process");
  ast->list_value = init_list(sizeof(AST_T*));
  ast->map = init_map(128);
  ast->writable = 1;
  gc_mark(FJB_ENV->GC, ast);

  AST_T* envobject = init_ast(AST_OBJECT);
  envobject->list_value = init_list(sizeof(AST_T*));
  envobject->name = strdup("env");
  envobject->map = init_map(128);
  envobject->writable = 1;
  gc_mark(FJB_ENV->GC, envobject);
  map_set(ast->map, "env", envobject);

  AST_T* default_node_env = init_ast(AST_STRING);
  default_node_env->string_value = strdup("dev");
  map_set(envobject->map, "NODE_ENV", default_node_env);

  AST_T* assignment = init_assignment("process", ast);
  assignment->writable = 1;
  map_set(FJB_ENV->map, "process", assignment);

  FJB_ENV->process = ast;
}

void init_fjb_env()
{
  FJB_ENV = calloc(1, sizeof(struct FJB_ENV_STRUCT));
  FJB_ENV->source = 0;
  FJB_ENV->filepath = 0;
  FJB_ENV->search_index = NEW_STACK;
  FJB_ENV->hooks = init_list(sizeof(plugin_hook));
  FJB_ENV->resolved_imports = init_list(sizeof(char*));
  FJB_ENV->imports = NEW_MAP();
  FJB_ENV->functions = NEW_MAP();
  FJB_ENV->assignments = NEW_MAP();
  FJB_ENV->map = NEW_MAP();
  FJB_ENV->GC = init_gc();

  register_process_object();

  /* ==== Built-in globals ==== */
  AST_T* module = init_ast(AST_OBJECT);
  module->name = strdup("module");
  module->list_value = init_list(sizeof(AST_T*));
  gc_mark(FJB_ENV->GC, module);

  AST_T* module_assignment = init_assignment("module", module);
  gc_mark(FJB_ENV->GC, module_assignment);

  AST_T* exports = init_ast(AST_OBJECT);
  exports->list_value = init_list(sizeof(AST_T*));
  exports->name = strdup("exports");
  gc_mark(FJB_ENV->GC, exports);

  AST_T* exports_assignment = init_assignment("exports", exports);
  gc_mark(FJB_ENV->GC, exports_assignment);
  list_push(module->list_value, exports_assignment);
}

void destroy_fjb_env()
{
  gc_sweep(FJB_ENV->GC);
  gc_free(FJB_ENV->GC);

 // if (FJB_ENV->imports)
  //  map_free(FJB_ENV->imports);
  //if (FJB_ENV->functions)
  //  map_free(FJB_ENV->functions);
  //if (FJB_ENV->assignments)
  //  map_free(FJB_ENV->assignments);
//  if (FJB_ENV->source)
//    free(FJB_ENV->source);
 // if (FJB_ENV->filepath)
  //  free(FJB_ENV->filepath);

  //free(FJB_ENV);
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

char* fjb_get_node_env()
{
  AST_T* env = (AST_T*)map_get_value(FJB_ENV->process->map, "env");

  if (!env)
    return 0;

  AST_T* node_env = (AST_T*)map_get_value(env->map, "NODE_ENV");

  if (!node_env)
    return 0;
  if (!node_env->string_value)
    return 0;

  return node_env->string_value;
}
