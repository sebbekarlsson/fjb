#include "include/fjb.h"
#include "include/compound.h"
#include "include/emit.h"
#include "include/emit_hooks.h"
#include "include/env.h"
#include "include/gc.h"
#include "include/io.h"
#include "include/js/headers.js.h"
#include "include/js/jsx_headers.js.h"
#include "include/js/optional_chain_headers.js.h"
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

compiler_result_T* _fjb()
{
  if (!FJB_ENV->source)
    return 0;

  FJB_ENV->ticks += 1;
  fjb_set_filepath(fjb_call_all_hooks(HOOK_RECEIVE_FILEPATH, FJB_ENV->filepath, FJB_ENV));

  if (FJB_ENV->source)
    fjb_set_source(fjb_call_all_hooks(HOOK_RECEIVE_SOURCE, FJB_ENV->source, FJB_ENV));

  NOOP = init_ast(AST_NOOP);
  gc_mark(FJB_ENV->GC, NOOP);

  /* ==== Lexing ==== */
  lexer_T* lexer = init_lexer(FJB_ENV->source, FJB_ENV->filepath);
#ifdef DEBUG
  printf("/* Lexer done. */\n");
#endif

  /* ==== Parsing ==== */
  parser_T* parser = init_parser(lexer, FJB_ENV);
  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);
#ifdef DEBUG
  printf("/* Parser done. */\n");
#endif

  if (!stack) {
    stack = NEW_STACK;
  }

  /* ==== Evaluate ==== */
  visitor_T* visitor = init_visitor(parser);
  root = visitor_visit(visitor, root, stack);
#ifdef DEBUG
  printf("/* Evaluation done. */\n");
#endif

  if (!root) {
    printf("No root node was generated.\n");
    exit(1);
  }

  /* ==== Tree-shake ==== */
  AST_T* root_to_emiterate = new_compound(root, FJB_ENV);
#ifdef DEBUG
  printf("/* Tree-shake done. */\n");
#endif

  /* ==== Generate ==== */
  char* str = strdup("");

  str = str_append(&str, "/* IMPORT `");
  str = str_append(&str, FJB_ENV->filepath);
  str = str_append(&str, "` */ ");
  char* out = emit(root_to_emiterate, FJB_ENV);

  if (out) {
    str = str_append(&str, strdup(out));
    free(out);
  }

  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = str ? str : strdup(" ");
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

compiler_result_T* fjb()
{
  if (!FJB_ENV->filepath)
    return 0;

  if (FJB_ENV->filepath)
    fjb_set_filepath(fjb_call_all_hooks(HOOK_RECEIVE_FILEPATH, FJB_ENV->filepath, FJB_ENV));

  compiler_result_T* result = _fjb();
  if (!result)
    return 0;

  char* headers = fjb_get_headers(FJB_ENV);

  result->headers = strdup(" ");

  char* imps = 0;
  if (FJB_ENV->compiled_imports) {
    char** keys = 0;
    unsigned int len = 0;
    map_get_keys(FJB_ENV->compiled_imports, &keys, &len);

    for (unsigned int i = 0; i < len; i++) {
      char* key = keys[i];
      if (!key)
        continue;

      AST_T* imp_res = (AST_T*)map_get_value(FJB_ENV->compiled_imports, key);
      if (!imp_res)
        continue;

      imp_res->dead = 0;

      char* impstr = emit(imp_res, FJB_ENV);
      if (!impstr)
        continue;
      imps = str_append(&imps, impstr);
    }
  }

  if (imps)
    headers = str_append(&headers, imps);

  if (headers) {
    const char* template = "%s\n%s";
    char* buff =
      calloc(strlen(headers) + strlen(result->stdout) + strlen(template) + 1, sizeof(char));

    if (buff) {
      sprintf(buff, template, headers, result->stdout);
      result->stdout = buff;
    }
  }

  return result;
}

char* fjb_get_headers(fjb_env_T* env)
{
  char* str = 0;

  str = str_append(&str, strdup((char*)_tmp_headers_js));

  if (FJB_ENV->is_using_optional_chain) {
    str = str_append(&str, strdup((char*)_tmp_optional_chain_headers_js));
  }

  if (FJB_ENV->is_using_jsx) {
    str = str_append(&str, strdup((char*)_tmp_jsx_headers_js));
  }

  return strdup(str ? str : " ");
}
