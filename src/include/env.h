#ifndef FJB_ENV_H
#define FJB_ENV_H
#include "AST.h"
#include "list.h"
#include <hashmap/map.h>

typedef struct FJB_ENV_STRUCT
{
  unsigned int is_using_jsx;
  unsigned int is_using_optional_chain;
  unsigned int aliased_import;
  unsigned long int level;
  unsigned long int ticks;

  AST_T* root;

  char* source;
  char* filepath;
  char* dumped_tree;

  map_T* imports;
  map_T* map;
  map_T* functions;
  map_T* assignments;

  AST_T* process;

  list_T* hooks;

  list_T* resolved_imports;

  AST_T* current;
  AST_T* current_import;

  map_T* compiled_imports;

  struct FJB_LEXER_STRUCT* lexer;
  struct FJB_PARSER_STRUCT* parser;

  gc_T* GC;
} fjb_env_T;

void init_fjb_env();

void destroy_fjb_env();

void fjb_set_source(char* source);

void fjb_set_filepath(char* filepath);

void fjb_set_aliased_import(unsigned int aliased_import);

list_T* fjb_get_hooks();

void* fjb_call_all_hooks(int type, void* ptr, fjb_env_T* env);

char* fjb_get_node_env();

int fjb_get_jsx_type();

void fjb_set_jsx_type(int jsx_type);
#endif
