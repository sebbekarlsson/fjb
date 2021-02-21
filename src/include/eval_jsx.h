#ifndef FJB_EVAL_JSX_H
#define FJB_EVAL_JSX_H
#include "AST.h"
#include "visitor.h"
AST_T* eval_jsx(visitor_T* visitor, AST_T* ast, list_T* stack);
#endif
