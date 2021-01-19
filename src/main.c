#include "include/io.h"
#include "include/fjb.h"
#include "include/gc.h"
#include <string.h>
#include <stdio.h>

gc_T* GC = 0;

int main(int argc, char* argv[]) {
  GC = init_gc();

  char* contents = fjb_read_file(argv[1]);
  compiler_result_T* result = fjb((GEN_FLAGS){argv[1], 0, 0}, contents, init_list(sizeof(AST_T*)));

  printf("%s\n", result->stdout);
  
  gc_sweep(GC);
  gc_free(GC);

  if (result->es_exports)
  {
    free(result->es_exports);
  }

  compiler_result_free(result);
  free(contents);

  return 0;
}
