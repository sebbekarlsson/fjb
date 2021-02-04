#include "include/test_utils.h"
#include <fjb/fjb.h>
#include <fjb/io.h>
#include <fjb/lexer.h>
#include <fjb/parser.h>
#include <fjb/resolve.h>
#include <fjb/string_utils.h>
#include <stdio.h>
#include <string.h>

#define OK() printf(GREEN "\t[OK]\n" RESET)

#define MSG() printf("\t%s\n", msg)

extern fjb_env_T* FJB_ENV;

unsigned int node_query(AST_T* ast, query_T query)
{
  char* name = ast_get_string(ast);

  if (query.name && !name)
    return 0;

  return (query.name ? strcmp(name, query.name) == 0 : 1) && ast->type == query.type;
}

void assert_node_exists(AST_T* root, int type, char* name, const char* msg)
{
  MSG();

  query_T query;
  query.name = name;
  query.type = type;
  AST_T* node = resolve(root, node_query, query);

  if (!node) {
    printf(RED "[FAIL]: Did NOT find node which was expected.\n"
               "\tname: %s, type: %d\n" RESET,
           name, type);
    exit(1);
  }

  OK();
}

void assert_node_not_exists(AST_T* root, int type, char* name, const char* msg)
{
  MSG();

  query_T query;
  query.name = name;
  query.type = type;
  AST_T* node = resolve(root, node_query, query);

  if (node) {
    printf(RED "[FAIL]: Found node which was NOT expected: %p\n"
               "\tname: %s, type: %d\n" RESET,
           node, name, type);
    exit(1);
  }

  OK();
}

AST_T* run_get_ast(const char* filepath, unsigned int post_process)
{
  char* contents = fjb_read_file(filepath);
  fjb_set_source(contents);
  fjb_set_filepath((char*)filepath);

  compiler_result_T* result = fjb();

  if (!post_process)
    return result->node;

  fjb_set_source(result->stdout);
  lexer_T* lexer = init_lexer(result->stdout, filepath);
  parser_T* parser = init_parser(lexer, FJB_ENV);

  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);

  return root;
}
