#ifndef FJB_VISITOR_H
#define FJB_VISITOR_H
#include "AST.h"
#include "parser.h"

typedef struct FJB_VISITOR_STRUCT {
  parser_T* parser;
} visitor_T;

visitor_T* init_visitor(parser_T* parser);

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* args);

#endif
