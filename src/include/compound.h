#ifndef FJB_COMPOUND_H
#define FJB_COMPOUND_H
#include "AST.h"
#include "flags.h"
#include "list.h"
AST_T* new_compound(AST_T* lookup, compiler_flags_T* flags);
#endif
