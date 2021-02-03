#ifndef FJB_ENV_H
#define FJB_ENV_H
#include "AST.h"
#include "list.h"
#include <hashmap/map.h>
typedef struct FJB_ENV_STRUCT
{
  unsigned int is_using_jsx;
  unsigned int has_included_jsx_headers;
  unsigned int results_changed;
  unsigned int aliased_import;

  AST_T* root;
  AST_T* module;
  AST_T* exports;

  list_T* imported_symbols;
  list_T* import_nodes;
  list_T* search_index;
  map_T* functions;
  map_T* assignments;
  list_T* imports;
  list_T* es_exports;

  char* source;
  char* filepath;
  char* dumped_tree;

  list_T* hooks;

  gc_T* GC;
} fjb_env_T;

void init_fjb_env();

void destroy_fjb_env();

void fjb_set_source(char* source);

void fjb_set_filepath(char* filepath);

void fjb_set_aliased_import(unsigned int aliased_import);

void fjb_set_only_parse(unsigned int only_parse);

list_T* fjb_get_hooks();

void* fjb_call_all_hooks(int type, void* ptr, fjb_env_T* env);
#endif
