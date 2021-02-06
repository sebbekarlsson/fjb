#ifndef FJB_ENV_H
#define FJB_ENV_H
#include "AST.h"
#include "list.h"
#include <hashmap/map.h>
typedef struct FJB_ENV_STRUCT
{
  unsigned int is_using_jsx;
  unsigned int has_included_jsx_headers;
  unsigned int has_included_headers;
  unsigned int results_changed;
  unsigned int aliased_import;
  unsigned int level;

  AST_T* root;

  list_T* search_index;

  char* source;
  char* filepath;
  char* dumped_tree;

  map_T* imports;
  map_T* map;
  map_T* functions;
  map_T* assignments;

  AST_T* process;

  unsigned int only_parse;

  list_T* hooks;

  list_T* resolved_imports;

  AST_T* current;

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

char* fjb_get_node_env();

int fjb_get_jsx_type();
#endif
