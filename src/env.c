#include "include/env.h"
#include <hooks/register.h>
#include "include/jsx.h"
#include "include/plugin.h"
#include "include/result.h"
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

  AST_T* default_jsx_type = init_ast(AST_STRING);
  default_node_env->string_value = strdup("jsx");
  map_set(envobject->map, "jsx", default_jsx_type);

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
  FJB_ENV->hooks = init_list(sizeof(plugin_hook));
  FJB_ENV->resolved_imports = init_list(sizeof(char*));
  FJB_ENV->imports = init_map(32);
  FJB_ENV->functions = NEW_MAP();
  FJB_ENV->assignments = NEW_MAP();
  FJB_ENV->map = NEW_MAP();
  FJB_ENV->GC = init_gc();

  register_process_object();

  FJB_ENV->compiled_imports = NEW_MAP();

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

  register_builtin_hooks();
}

void destroy_fjb_env()
{
  gc_sweep(FJB_ENV->GC);
  gc_free(FJB_ENV->GC);

  if (FJB_ENV->imports)
    map_free(FJB_ENV->imports);
  if (FJB_ENV->functions)
    map_free(FJB_ENV->functions);
  if (FJB_ENV->assignments)
    map_free(FJB_ENV->assignments);
  if (FJB_ENV->source)
    free(FJB_ENV->source);
  if (FJB_ENV->filepath)
    free(FJB_ENV->filepath);

  free(FJB_ENV);
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

void fjb_set_outfile(char* filepath)
{
  if (!filepath)
    return;

  FJB_ENV->outfile = filepath ? strdup(filepath) : 0;
}

list_T* fjb_get_hooks()
{
  return FJB_ENV->hooks;
}

void* fjb_call_all_hooks(int type, void* ptr)
{
  for (unsigned int i = 0; i < FJB_ENV->hooks->size; i++) {
    plugin_hook hook = (plugin_hook)FJB_ENV->hooks->items[i];
    if (!hook)
      continue;

    ptr = hook(type, ptr, FJB_ENV);
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

int fjb_get_jsx_type()
{
  AST_T* env = (AST_T*)map_get_value(FJB_ENV->process->map, "env");

  if (!env)
    return 0;

  AST_T* jsxval = (AST_T*)map_get_value(env->map, "jsx");

  if (!jsxval)
    return 0;
  if (!jsxval->string_value)
    return 0;

  if (strcmp(jsxval->string_value, "react") == 0)
    return JSX_REACT;

  return JSX_DEFAULT;
}

void fjb_set_jsx_type(int jsx_type)
{
  AST_T* env = (AST_T*)map_get_value(FJB_ENV->process->map, "env");

  if (!env)
    return;

  AST_T* jsxval = (AST_T*)map_get_value(env->map, "jsx");

  if (jsxval)
    gc_mark(FJB_ENV->GC, jsxval);

  AST_T* val = init_ast_string(jsx_type == JSX_REACT ? "react" : "jsx");

  map_set(env->map, "jsx", val);
}

unsigned int fjb_ast_is_imported(AST_T* ast)
{
  if (!ast)
    return 0;

  return (ast->name && (map_get(FJB_ENV->imports, ast->name) != 0 ||
                        map_get(FJB_ENV->compiled_imports, ast->name) != 0) ||
          (ast->left && ast->left->name &&
           (map_get(FJB_ENV->imports, ast->left->name) != 0 ||
            map_get(FJB_ENV->compiled_imports, ast->left->name) != 0)));
}

unsigned int fjb_ast_should_be_exposed(AST_T* ast)
{
  if (!ast)
    return 0;

  return (fjb_ast_is_imported(ast) || ast->exported) && !ast->anon;
}

imported_T* fjb_get_imported(char* name)
{
  if (!name)
    return 0;
  return (imported_T*)map_get_value(FJB_ENV->imports, name);
}

void fjb_register_function(AST_T* ast, char* name)
{
  if (!name)
    return;

  map_set(FJB_ENV->functions, strdup(ast->name), ast);
  map_set(FJB_ENV->map, strdup(ast->name), ast);
}

void fjb_register_assignment(AST_T* ast, char* name)
{
  if (!name)
    return;

  map_set(FJB_ENV->assignments, strdup(name), ast);
  map_set(FJB_ENV->map, strdup(name), ast);
}
