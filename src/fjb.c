#include "include/fjb.h"
#include "include/compound.h"
#include "include/emit.h"
#include "include/emit_hooks.h"
#include "include/env.h"
#include "include/gc.h"
#include "include/io.h"
#include "include/js/jsx_headers.js.h"
#include "include/lexer.h"
#include "include/list.h"
#include "include/parser.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include "include/visitor.h"
#include <stdio.h>
#include <string.h>

AST_T* NOOP;
list_T* stack;
extern fjb_env_T* FJB_ENV;

compiler_result_T* fjb()
{
  if (!FJB_ENV->source)
    return 0;

  unsigned int old_level = FJB_ENV->level;
  NOOP = init_ast(AST_NOOP);
  gc_mark(FJB_ENV->GC, NOOP);

  /* ==== Lexing ==== */
  lexer_T* lexer = init_lexer(FJB_ENV->source, FJB_ENV->filepath);
  parser_T* parser = init_parser(lexer, FJB_ENV);

  /* ==== Parsing ==== */
  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);

  if (!stack) {
    stack = NEW_STACK;
  }

  /* ==== Evaluate ==== */
  visitor_T* visitor = init_visitor(parser);
  root = visitor_visit(visitor, root, stack);

  if (!root) {
    printf("No root node was generated.\n");
    exit(1);
  }

  /* ==== Tree-shake ==== */
  AST_T* root_to_emiterate = new_compound(root, FJB_ENV);

  /* ==== Generate ==== */
  char* str = strdup("");

  str = str_append(&str, "/* IMPORT `");
  str = str_append(&str, FJB_ENV->filepath);
  str = str_append(&str, "` */ ");
  char* out = emit(root_to_emiterate, FJB_ENV);

  if (out) {
    str = str_append(&str, out);
    free(out);
  }

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = str;
  FJB_ENV->output = str;
  result->headers = fjb_get_headers(FJB_ENV);
  result->node = root;
  FJB_ENV->root = root_to_emiterate;

  if (FJB_ENV->filepath)
    result->filepath = strdup(FJB_ENV->filepath);

  lexer_free(lexer);
  parser_free(parser);
  visitor_free(visitor);

  if (root_to_emiterate != root) {
    root_to_emiterate->list_value = 0;
    gc_mark(FJB_ENV->GC, root_to_emiterate);
  }

  FJB_ENV->level += 1;

  result = fjb_call_all_hooks(HOOK_BEFORE_COMPILE, result, FJB_ENV);

  return result;
}

char* fjb_get_headers(fjb_env_T* env)
{
  char* str = 0;
  if (FJB_ENV->is_using_jsx && !FJB_ENV->has_included_jsx_headers) {
    str = str_append(&str, (const char*)_tmp_jsx_headers_js);
    FJB_ENV->has_included_jsx_headers = 1;
  }

  return str;
}

void fjb_set_only_parse(unsigned int only_parse)
{
  FJB_ENV->only_parse = 1;
}
