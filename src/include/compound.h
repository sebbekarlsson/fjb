#ifndef FJB_COMPOUND_H
#define FJB_COMPOUND_H
#include "AST.h"
#include "flags.h"
#include "list.h"
AST_T* new_compound(AST_T* lookup, compiler_flags_T* flags);
list_T* get_imported_symbols(AST_T* lookup, list_T* imports, list_T* search_index);
#endif
