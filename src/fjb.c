#include "include/fjb.h"
#include "include/compound.h"
#include "include/gc.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/string_utils.h"
#include "include/visitor.h"
#include <stdio.h>
#include <string.h>

extern gc_T* GC;
AST_T* NOOP;
extern volatile unsigned int dump;

list_T* stack;

compiler_result_T* fjb(gen_flags_T flags, char* source, list_T* imports)
{
  if (!source)
    return 0;

  NOOP = init_ast(AST_NOOP);
  gc_mark(GC, NOOP);

  list_T* search_index = NEW_STACK;

  /* ==== Lexing ==== */
  lexer_T* lexer = init_lexer(source, flags.filepath);
  parser_T* parser = init_parser(lexer, flags.filepath, search_index);

  /* ==== Parsing ==== */
  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);

  /* ==== Built-in globals ==== */
  AST_T* module = init_ast(AST_OBJECT);
  module->name = strdup("module");
  module->list_value = init_list(sizeof(AST_T*));
  gc_mark(GC, module);

  AST_T* exports = init_ast(AST_OBJECT);
  exports->list_value = init_list(sizeof(AST_T*));
  exports->name = strdup("exports");
  gc_mark(GC, exports);

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  assignment->name = strdup("module");
  assignment->value = module;
  gc_mark(GC, assignment);

  AST_T* exports_assignment = init_ast(AST_ASSIGNMENT);
  exports_assignment->name = strdup("exports");
  exports_assignment->value = exports;
  gc_mark(GC, exports_assignment);

  list_push(module->list_value, exports_assignment);

  if (!stack) {
    stack = NEW_STACK;
  }

  list_push(stack, exports_assignment);
  list_push(stack, assignment);

  /* ==== Evaluate ==== */
  visitor_T* visitor = init_visitor(parser, flags.filepath, imports, module, exports);
  visitor->pre_loaded_symbols = NEW_STACK;
  gc_mark_list(GC, visitor->pre_loaded_symbols);
  root = visitor_visit(visitor, root, stack);

  /* ==== Tree-shake ==== */
  list_T* es_exports = NEW_STACK;
  gc_mark_list(GC, es_exports);
  AST_T* root_to_generate = new_compound(root, visitor->imports, es_exports, search_index);

  /* ==== Generate ==== */
  flags.exports = exports;
  flags.imports = imports;
  char* str = 0;

  str = str_append(&str, "/* IMPORTED FROM `");
  str = str_append(&str, flags.filepath);
  str = str_append(&str, "` */\n");
  char* out = gen(root_to_generate, flags);
  str = str_append(&str, out);
  free(out);

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = str;
  result->es_exports = es_exports;
  result->node = root_to_generate;
  
  if (dump)
  {
    char* dumped = visitor->dumped;
    char* newdump = _ast_to_str(result->node, 0);

    dumped = str_append(&dumped, newdump);

    result->dumped = dumped;
  }

  lexer_free(lexer);
  parser_free(parser);
  visitor_free(visitor);

  if (root_to_generate != root) {
    root_to_generate->list_value = 0;
    gc_mark(GC, root_to_generate);
  }

  return result;
}

void compiler_result_free(compiler_result_T* result)
{
  if (!result)
    return;

  if (result->stdout)
    free(result->stdout);

  free(result);
}
