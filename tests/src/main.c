#include "include/test_utils.h"
#include <fjb/AST.h>
#include <fjb/env.h>
#include <stdio.h>

#define MSG() printf(BOLDWHITE "Testing: %s\n" RESET, filepath)

void test_es6()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/es6/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_simple()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/simple/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "subtract", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_lodash()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/with_lodash/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "createRound", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_vue()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/with_vue/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_STATE, "new", "test AST_STATE exists");
  destroy_fjb_env();
}

void test_highlightjs()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/highlightjs/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_ASSIGNMENT, "hljs", "test AST_ASSIGNMENT exists");
  destroy_fjb_env();
}

void test_with_jquery()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/with_jquery/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);

  assert_node_exists(root, AST_ASSIGNMENT, "$", "test AST_ASSIGNMENT exists");
  destroy_fjb_env();
}

void test_no_imports()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/noimports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);

  assert_node_exists(root, AST_FUNCTION, "hello", "test AST_FUNCTION exists");
  destroy_fjb_env();
}

void test_with_assignment()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/with_assignments/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);

  assert_node_exists(root, AST_ASSIGNMENT, "myfunc", "test arrow function assignment exists");
  destroy_fjb_env();
}

void test_strings()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/strings/index.js";

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
  const char* filepath = "fjb-samples/regex/index.js";

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
  const char* filepath = "fjb-samples/class/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_CLASS, "Animal", "test AST_CLASS Animal exists.");
  assert_node_exists(root, AST_CLASS, "Dog", "test AST_CLASS Dog exists.");
  destroy_fjb_env();
}

void test_alias_imports()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/alias_imports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_ASSIGNMENT, "mul", "test AST_ASSIGNMENT mul exists.");

  destroy_fjb_env();
}

void test_wildcard_imports()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/wildcard_imports/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_ASSIGNMENT, "utils", "test AST_ASSIGNMENT utils exists.");

  destroy_fjb_env();
}

void test_jsx()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/jsx/index.jsx";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_NAME, "document", "test AST_NAME document exists.");

  destroy_fjb_env();
}

void test_jsx_custom()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/jsx_custom/index.jsx";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_NAME, "document", "test AST_NAME document exists.");
  assert_node_exists(root, AST_NAME, "HeadTitle", "test AST_CALL HeadTitle exists.");
  destroy_fjb_env();
}

void test_json_import()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/json_import/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_exists(root, AST_ARRAY, 0, "test AST_ARRAY exists.");
  destroy_fjb_env();
}

void test_css_import()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/css_import/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_not_exists(root, AST_TEMPLATE_STRING, 0, "test AST_TEMPLATE_STRING exists.");
  destroy_fjb_env();
}

void test_scss_import()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/scss_import/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 1);
  assert_node_not_exists(root, AST_TEMPLATE_STRING, 0, "test AST_TEMPLATE_STRING exists.");
  destroy_fjb_env();
}

void test_destruct()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/destruct/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_FUNCTION, "xyz", "test AST_FUNCTION exists.");
  assert_node_exists(root, AST_CALL, "xyz", "test AST_CALL exists.");
  destroy_fjb_env();
}

void test_ternary()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ternary/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_SWITCH, 0, "test AST_SWITCH exists.");
  assert_node_exists(root, AST_TERNARY, 0, "test AST_TERNARY exists.");
  assert_node_exists(root, AST_OBJECT, 0, "test AST_OBJECT exists.");
  destroy_fjb_env();
}

void test_array_access()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/array_access/index.js";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_FUNCTION, 0, "test AST_FUNCTION exists.");
  assert_node_exists(root, AST_NAME, "a", "test AST_NAME exists.");
  assert_node_exists(root, AST_NAME, "b", "test AST_NAME exists.");
  assert_node_exists(root, AST_NAME, "c", "test AST_NAME exists.");
  destroy_fjb_env();
}

void test_typescript_class()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ts_fib/index.ts";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_CLASS, "Fibonacci", "test AST_CLASS exists.");
  destroy_fjb_env();
}

void test_typescript_union()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ts_union/index.ts";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_NAME, "firstname", "test AST_NAME exists.");
  destroy_fjb_env();
}

void test_typescript_intersect()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ts_intersect/index.ts";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_NAME, "ab", "test AST_NAME exists.");
  destroy_fjb_env();
}

void test_typescript_interface_extends()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ts_interface_extends/index.ts";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_COMPOUND, 0, "test AST_COMPOUND exists.");
  destroy_fjb_env();
}

void test_typescript_generic()
{
  init_fjb_env();
  const char* filepath = "fjb-samples/ts_generics/index.ts";

  MSG();

  AST_T* root = run_get_ast(filepath, 0);
  assert_node_exists(root, AST_NAME, "book", "test AST_NAME exists.");
  destroy_fjb_env();
}

int main(int argc, char* argv[])
{
  test_es6();
  test_simple();
  test_with_lodash();
  test_highlightjs();
  test_with_jquery();
  test_with_vue();
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
  test_scss_import();
  test_destruct();
  test_ternary();
  test_array_access();
  test_typescript_class();
  test_typescript_union();
  test_typescript_intersect();
  test_typescript_interface_extends();
  test_typescript_generic();

  return 0;
}
