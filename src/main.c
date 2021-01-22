#include "include/fjb.h"
#include "include/gc.h"
#include "include/io.h"
#include <stdio.h>
#include <string.h>

gc_T* GC = 0;

int main(int argc, char* argv[])
{
  GC = init_gc();

  char* contents = fjb_read_file(argv[1]);
  gen_flags_T flags;
  flags.filepath = argv[1];
  list_T* es_exports = init_list(sizeof(AST_T*));
  gc_mark_list(GC, es_exports);
  compiler_result_T* result = fjb(flags, contents, es_exports);

  printf("%s\n", result->stdout);

  gc_sweep(GC);
  gc_free(GC);

  compiler_result_free(result);
  free(contents);

  return 0;
}
