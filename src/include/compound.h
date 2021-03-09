#ifndef FJB_COMPOUND_H
#define FJB_COMPOUND_H
#include "AST.h"
#include "env.h"
AST_T* new_compound(AST_T* lookup, fjb_env_T* env);
#endif
