#ifndef FJB_EVAL_H
#define FJB_EVAL_H
#include "AST.h"
#include "visitor.h"
AST_T* eval(visitor_T* visitor, AST_T* ast);
AST_T* eval_binop(visitor_T* visitor, AST_T* ast);
AST_T* eval_name(visitor_T* visitor, AST_T* ast);
#endif
