#include "../../src/include/AST.h"
#include "include/test_utils.h"
#include <stdio.h>

#define MSG() printf(BOLDWHITE "Testing: %s\n" RESET, filepath)

void test_es6()
{
  const char* filepath = "./src/test_projects/es6/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
}

void test_simple()
{
  const char* filepath = "./src/test_projects/simple/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
}

void test_with_lodash()
{
  const char* filepath = "./src/test_projects/with_lodash/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_ASSIGNMENT, "ceil", "test AST_ASSIGNMENT exists");
  assert_node_exists(root, AST_FUNCTION, "createRound", "test AST_FUNCTION exists");
}

void test_no_imports()
{
  const char* filepath = "./src/test_projects/noimports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_FUNCTION, "hello", "test AST_FUNCTION exists");
}

void test_with_assignment()
{
  const char* filepath = "./src/test_projects/with_assignments/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_ASSIGNMENT, "myfunc", "test arrow function assignment exists");
}

void test_strings()
{
  const char* filepath = "./src/test_projects/strings/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_STRING,
                     "*(?:'((?:\x5c\x5c\x5c\x5c.|[^\x5c\x5c\x5c\x5c'])*)'|\x5c\"((?:"
                     "\x5c\x5c\x5c\x5c.|[^\x5c\x5c\x5c\x5c\x5c\"])*)\x5c\"|(",
                     "test string escaping is correct");
}

void test_regex()
{
  const char* filepath = "./src/test_projects/regex/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);

  assert_node_exists(root, AST_FUNCTION, "validateEmail", "test AST_FUNCTION exists.");
  assert_node_exists(root, AST_REGEX, "/\x5cS+@\x5cS+\x5c.\x5cS+/",
                     "test regex exists and is correct.");
}

void test_class()
{
  const char* filepath = "./src/test_projects/class/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath);
  assert_node_exists(root, AST_CLASS, "Animal", "test AST_CLASS Animal exists.");
  assert_node_exists(root, AST_CLASS, "Dog", "test AST_CLASS Dog exists.");
}

int main(int argc, char* argv[])
{
  test_es6();
  test_simple();
  test_with_lodash();
  test_no_imports();
  test_with_assignment();
  test_strings();
  test_regex();
  test_class();

  return 0;
}
