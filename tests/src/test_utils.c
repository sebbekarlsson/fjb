#include "include/test_utils.h"
#include "../../src/include/fjb.h"
#include "../../src/include/io.h"
#include "../../src/include/lexer.h"
#include "../../src/include/parser.h"
#include "../../src/include/resolve.h"
#include <stdio.h>
#include <string.h>

#define OK() printf("\t[OK]\n")

#define MSG() printf("\t%s\n", msg)

unsigned int node_query(AST_T* ast, query_T query)
{
  if (!ast->name)
    return 0;

  return strcmp(ast->name, query.name) == 0 && ast->type == query.type;
}

void assert_node_exists(AST_T* root, int type, char* name, const char* msg)
{
  MSG();

  query_T query;
  query.name = name;
  query.type = type;
  AST_T* node = resolve(root, node_query, query);

  if (!node) {
    printf("[FAIL]: Did NOT find node which was expected.\n"
           "\tname: %s, type: %d\n",
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
    printf("[FAIL]: Found node which was NOT expected: %p\n"
           "\tname: %s, type: %d\n",
           node, name, type);
    exit(1);
  }

  OK();
}

AST_T* run_get_ast(const char* filepath)
{
  char* contents = fjb_read_file(filepath);
  compiler_flags_T* flags = init_compiler_flags(contents, (char*)filepath, 0);
  compiler_result_T* result = fjb(flags);

  lexer_T* lexer = init_lexer(result->stdout, filepath);
  parser_T* parser = init_parser(lexer, flags);

  parser_options_T options = EMPTY_PARSER_OPTIONS;
  AST_T* root = parser_parse(parser, options);

  return root;
}
