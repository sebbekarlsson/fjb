#include "include/fjb.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/visitor.h"
#include "include/io.h"
#include "include/gen.h"
#include <string.h>
#include <stdio.h>

compiler_result_T* fjb(GEN_FLAGS flags, char *source)
{
  lexer_T* lexer = init_lexer(source, flags[0]);
  parser_T* parser = init_parser(lexer, flags[0]);
  AST_T* root = parser_parse(parser);
  visitor_T* visitor = init_visitor(parser);

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  assignment->name = strdup("module");

  AST_T* exports = init_ast(AST_OBJECT);
  exports->list_value = init_list(sizeof(AST_T*));

  AST_T* exports_assignment = init_ast(AST_ASSIGNMENT);
  exports_assignment->name = strdup("exports");
  exports_assignment->value = exports; 

  AST_T* module = init_ast(AST_OBJECT);

  assignment->value = module;

  module->list_value = init_list(sizeof(AST_T*));
  list_push(module->list_value, exports_assignment);
  
  list_T* args = init_list(sizeof(AST_T*));
  list_push(args, assignment);

  AST_T* transformed = visitor_visit(visitor, root, args);

  char* out = gen(transformed, flags);

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = out;
  result->args = args;

  return result;
}
