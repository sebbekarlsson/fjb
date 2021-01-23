/**
 * TODO: write better tests where we actually inspect
 * the AST to find expected nodes.
 */
#include "../../src/include/fjb.h"
#include "../../src/include/gc.h"
#include "../../src/include/io.h"
#include "../../src/include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gc_T* GC;

char* run(const char* filepath)
{
  GC = init_gc();

  char* contents = fjb_read_file(filepath);
  gen_flags_T flags;
  flags.filepath = (char*)filepath;
  list_T* es_exports = init_list(sizeof(AST_T*));
  gc_mark_list(GC, es_exports);
  compiler_result_T* result = fjb(flags, contents, es_exports);

  return remove_whitespace(result->stdout);
}

void ASSERT(unsigned int expr, const char* msg)
{
  if (!expr) {
    printf("[TEST FAILED]: (code: %d) %s\n", (int)expr, msg);
    exit(1);
  }

  printf("[TEST OK]: %s\n", msg);
}

void test_es6()
{
  const char* ENTRY_POINT = "src/test_projects/es6/index.js";
  const char* DIST = "src/test_projects/es6/dist.js";

  char* r = run(ENTRY_POINT);

  char* expected = remove_whitespace(fjb_read_file(DIST));

  ASSERT(strcmp(r, expected) == 0, ENTRY_POINT);
}

void test_simple()
{
  const char* ENTRY_POINT = "src/test_projects/simple/index.js";
  const char* DIST = "src/test_projects/simple/dist.js";

  char* r = run(ENTRY_POINT);

  char* expected = remove_whitespace(fjb_read_file(DIST));

  ASSERT(strcmp(r, expected) == 0, ENTRY_POINT);
}

int main(int argc, char* argv[])
{
  test_es6();
  test_simple();
  return 0;
}
