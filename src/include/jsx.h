#ifndef FJB_JSX_H
#define FJB_JSX_H
#include "parser.h"

enum JSX_TYPES
{
  JSX_DEFAULT,
  JSX_REACT
};

AST_T* parse_jsx(parser_T* parser, parser_options_T options);
AST_T* parse_jsx_attr_value(parser_T* parser, parser_options_T options);
AST_T* parse_jsx_compute_block(parser_T* parser, parser_options_T options);
#endif
