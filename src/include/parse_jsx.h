#ifndef FJB_PARSE_JSX_H
#define FJB_PARSE_JSX_H
#include "parser.h"

AST_T* parse_jsx(parser_T* parser, parser_options_T options);
AST_T* parse_jsx_attr_value(parser_T* parser, parser_options_T options);
AST_T* parse_jsx_compute_block(parser_T* parser, parser_options_T options);
#endif
