#ifndef FJB_SIGNALS_H
#define FJB_SIGNALS_H
#include "AST.h"
#include "list.h"
typedef struct FJB_SIGNALS_STRUCT
{
  unsigned int is_using_jsx;
  unsigned int has_included_jsx_headers;
  unsigned int results_changed;
  AST_T* root;
  list_T* imports;
  list_T* imported_symbols;
  list_T* import_nodes;
} fjb_signals_T;

void init_fjb_signals();
#endif
