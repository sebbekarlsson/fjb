#include "include/test_utils.h"
#include <fjb/AST.h>
#include <fjb/env.h>
#include <stdio.h>

#define MSG() printf(BOLDWHITE "Testing: %s\n" RESET, filepath)

void test_es6()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/es6/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_simple()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/simple/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_lodash()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/with_lodash/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  // assert_node_exists(root, AST_ASSIGNMENT, "ceil", "test AST_ASSIGNMENT exists");
  assert_node_exists(root, AST_FUNCTION, "createRound", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_jquery()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/with_jquery/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);

  assert_node_exists(root, AST_ASSIGNMENT, "$", "test AST_ASSIGNMENT exists");
  destroy_fjb_env();
}

void test_no_imports()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/noimports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "hello", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_assignment()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/with_assignments/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);

  assert_node_exists(root, AST_ASSIGNMENT, "myfunc", "test arrow function assignment exists");
  destroy_fjb_env();
}

void test_strings()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/strings/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_STRING,
                     "*(?:'((?:\x5c\x5c\x5c\x5c.|[^\x5c\x5c\x5c\x5c'])*)'|\x5c\"((?:"
                     "\x5c\x5c\x5c\x5c.|[^\x5c\x5c\x5c\x5c\x5c\"])*)\x5c\"|(",
                     "test string escaping is correct");
  destroy_fjb_env();
}

void test_regex()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/regex/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "validateEmail", "test AST_FUNCTION exists.");
  assert_node_exists(root, AST_REGEX, "/\x5cS+@\x5cS+\x5c.\x5cS+/",
                     "test regex exists and is correct.");
  destroy_fjb_env();
}

void test_class()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/class/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_CLASS, "Animal", "test AST_CLASS Animal exists.");
  assert_node_exists(root, AST_CLASS, "Dog", "test AST_CLASS Dog exists.");
  destroy_fjb_env();
}

void test_alias_imports()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/alias_imports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_ASSIGNMENT, "mul", "test AST_ASSIGNMENT mul exists.");

  destroy_fjb_env();
}

void test_wildcard_imports()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/wildcard_imports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_ASSIGNMENT, "utils", "test AST_ASSIGNMENT utils exists.");

  destroy_fjb_env();
}

void test_jsx()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/jsx/index.jsx";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_NAME, "document", "test AST_NAME document exists.");

  destroy_fjb_env();
}

void test_jsx_custom()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/jsx_custom/index.jsx";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_NAME, "document", "test AST_NAME document exists.");
  assert_node_exists(root, AST_NAME, "HeadTitle", "test AST_NAME HeadTitle exists.");
  destroy_fjb_env();
}

void test_json_import()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/json_import/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_not_exists(root, AST_ARRAY, 0, "test AST_ARRAY does not exists.");
  AST_T* root_after = run_get_ast(filepath, 1);
  assert_node_not_exists(root, AST_ARRAY, 0, "test AST_ARRAY exists.");
  destroy_fjb_env();
}

void test_css_import()
{
  init_fjb_env();
  const char* filepath = "./src/test_projects/css_import/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_not_exists(root, AST_TEMPLATE_STRING, 0, "test AST_TEMPLATE_STRING exists.");
  destroy_fjb_env();
}

int main(int argc, char* argv[])
{
  test_es6();
  test_simple();
  test_with_lodash();
  test_with_jquery();
  test_no_imports();
  test_with_assignment();
  test_strings();
  test_regex();
  test_class();
  test_alias_imports();
  test_wildcard_imports();
  test_jsx();
  test_jsx_custom();
  test_json_import();
  test_css_import();

  return 0;
}
