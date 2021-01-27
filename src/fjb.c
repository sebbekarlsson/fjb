#include "include/fjb.h"
#include "include/compound.h"
#include "include/gc.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/special_gen.h"
#include "include/string_utils.h"
#include "include/visitor.h"
#include <stdio.h>
#include <string.h>

AST_T* NOOP;
list_T* stack;

compiler_result_T* fjb(compiler_flags_T* flags)
{
  if (!flags->source)
    return 0;

  char* ext = get_filename_ext(flags->filepath);

  if (strcasecmp(ext, ".json") == 0) {
    return special_gen_json(flags);
  }

  NOOP = init_ast(AST_NOOP);
  gc_mark(flags->GC, NOOP);

  list_T* search_index = NEW_STACK;

  /* ==== Lexing ==== */
  lexer_T* lexer = init_lexer(flags->source, flags->filepath);
  parser_T* parser = init_parser(lexer, flags);

  /* ==== Parsing ==== */
  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);

  if (!stack) {
    stack = NEW_STACK;
  }

  /* ==== Evaluate ==== */
  visitor_T* visitor = init_visitor(parser, flags);
  visitor->pre_loaded_symbols = NEW_STACK;
  gc_mark_list(flags->GC, visitor->pre_loaded_symbols);
  root = visitor_visit(visitor, root, stack);

  /* ==== Tree-shake ==== */
  list_T* es_exports = NEW_STACK;
  gc_mark_list(flags->GC, es_exports);
  AST_T* root_to_generate = new_compound(root, flags);

  /* ==== Generate ==== */
  char* str = 0;

  str = str_append(&str, "/* IMPORTED FROM `");
  str = str_append(&str, flags->filepath);
  str = str_append(&str, "` */\n");
  char* out = gen(root_to_generate, flags);
  str = str_append(&str, out);
  free(out);

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = str;
  result->es_exports = flags->es_exports;
  result->node = root_to_generate;
  result->filepath = strdup(flags->filepath);

  if (flags->should_dump) {
    char* dumped = visitor->flags->dumped_tree;
    char* newdump = _ast_to_str(result->node, 0);

    dumped = str_append(&dumped, newdump);

    result->dumped = dumped;
  }

  lexer_free(lexer);
  parser_free(parser);
  visitor_free(visitor);

  if (root_to_generate != root) {
    root_to_generate->list_value = 0;
    gc_mark(flags->GC, root_to_generate);
  }

  return result;
}
