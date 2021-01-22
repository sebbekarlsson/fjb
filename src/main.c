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
  compiler_result_T* result = fjb(flags, contents, init_list(sizeof(AST_T*)));

  printf("%s\n", result->stdout);

  gc_sweep(GC);
  gc_free(GC);

  if (result->es_exports) {
    free(result->es_exports);
  }

  compiler_result_free(result);
  free(contents);

  return 0;
}
