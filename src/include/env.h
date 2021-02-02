#ifndef FJB_ENV_H
#define FJB_ENV_H
#include "../../external/hashmap/src/include/map.h"
#include "AST.h"
#include "list.h"
typedef struct FJB_ENV_STRUCT
{
  unsigned int is_using_jsx;
  unsigned int has_included_jsx_headers;
  unsigned int results_changed;
  unsigned int aliased_import;
  unsigned int level;

  AST_T* root;
  AST_T* module;
  AST_T* exports;

  list_T* search_index;

  char* source;
  char* filepath;
  char* dumped_tree;
  char* output;

  map_T* imports;
  map_T* functions;
  map_T* assignments;

  unsigned int only_parse;

  gc_T* GC;
} fjb_env_T;

void init_fjb_env();

void destroy_fjb_env();

void fjb_set_source(char* source);

void fjb_set_filepath(char* filepath);

void fjb_set_aliased_import(unsigned int aliased_import);

void fjb_set_only_parse(unsigned int only_parse);
#endif
