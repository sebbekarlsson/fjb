#ifndef FJB_PARSE_TYPESCRIPT_H
#define FJB_PARSE_TYPESCRIPT_H
#include "parser.h"
#include <list/list.h>
AST_T* parser_parse_interface(parser_T* parser, parser_options_T options);
AST_T* parser_parse_generic(parser_T* parser, parser_options_T options);
AST_T* parser_parse_typehints(parser_T* parser, parser_options_T options);
#endif
