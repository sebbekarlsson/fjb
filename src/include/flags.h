#ifndef FJB_FLAGS_H
#define FJB_FLAGS_H
#include "AST.h"
#include "gc.h"
#include "list.h"
typedef struct FJB_COMPILER_FLAGS
{
  char* source;
  char* filepath;
  char* dumped_tree;

  unsigned int should_dump;

  list_T* es_exports;
  AST_T* module;
  AST_T* exports;
  gc_T* GC;
  list_T* imports;
  unsigned int aliased_import;

  unsigned int only_parse;

  list_T* search_index;
} compiler_flags_T;

compiler_flags_T* init_compiler_flags(char* source, char* filepath, unsigned int should_dump);
#endif
