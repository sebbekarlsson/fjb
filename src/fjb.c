#include "include/fjb.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/visitor.h"
#include "include/io.h"
#include "include/gen.h"
#include "include/gc.h"
#include "include/string_utils.h"
#include <string.h>
#include <stdio.h>

extern gc_T* GC;
AST_T* NOOP;

list_T* defs;
list_T* stack;

compiler_result_T* fjb(GEN_FLAGS flags, char *source, list_T* refs, list_T* imports, unsigned int all)
{
  if (!defs) defs = init_list(sizeof(AST_T*));

  NOOP = init_ast(AST_NOOP);
  gc_mark(GC, NOOP);

  lexer_T* lexer = init_lexer(source, flags[0]);
  parser_T* parser = init_parser(lexer, flags[0]);
  
  parser_options_T options = EMPTY_PARSER_OPTIONS;

  AST_T* module = init_ast(AST_OBJECT);
  AST_T* exports = init_ast(AST_OBJECT);

  exports->name = strdup("exports");
  
  printf("/* Parsing...*/\n");
  AST_T* root = parser_parse(parser, options);

  visitor_T* visitor = init_visitor(parser, flags[0], refs, imports, module, exports, all);

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  assignment->name = strdup("module");

  exports->list_value = init_list(sizeof(AST_T*));
  
  gc_mark(GC, exports);

  AST_T* exports_assignment = init_ast(AST_ASSIGNMENT);
  exports_assignment->name = strdup("exports");
  exports_assignment->value = exports; 
  
  gc_mark(GC, exports_assignment);


  gc_mark(GC, module);

  assignment->value = module;
  module->list_value = init_list(sizeof(AST_T*));
  list_push(module->list_value, exports_assignment);
  
  list_T* args = init_list(sizeof(AST_T*));
  list_push(args, assignment);

  gc_mark(GC, assignment);

  list_T* imported_symbols = init_list(sizeof(AST_T*));
  
  if (!stack)
    stack = NEW_STACK;
  
  list_push(stack, assignment);

  printf("/* Visiting...*/\n");
  root = visitor_visit(visitor, root, args, stack);

  
  printf("/* Generating...*/\n");
  printf("/*\n");
  printf("\n=======================\n%s\n", ast_to_str(exports));
  printf("*/\n");

  char* str = 0;
  str = str_append(&str, "/* IMPORTED FROM `");
  str = str_append(&str, flags[0]);
  str = str_append(&str, "` */\n");
  char* out = gen(root, flags);
  str = str_append(&str, out);
  free(out);

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = str;
  result->args = args;
  result->es_exports = visitor->es_exports;

  lexer_free(lexer);
  parser_free(parser);
  visitor_free(visitor);

  return result;
}

void compiler_result_free(compiler_result_T* result)
{
  if (!result) return;

  if (result->args)
  {
    if (result->args->items)
      free(result->args->items);

    free(result->args);

    result->args = 0;
  }
  if (result->stdout)
    free(result->stdout);

  free(result);
}
