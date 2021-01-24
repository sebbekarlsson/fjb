#include "../../src/include/AST.h"
#include "include/test_utils.h"
#include <stdio.h>

#define MSG() printf("Testing: %s\n", filepath)

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

int main(int argc, char* argv[])
{
  test_es6();
  test_simple();
  test_with_lodash();

  return 0;
}
