#include "include/fjb.h"
#include "include/gc.h"
#include "include/io.h"
#include <stdio.h>
#include <string.h>

gc_T* GC = 0;

volatile unsigned int dump;

unsigned int getarg(int argc, char* argv[], const char* key)
{
  for (int i = argc == 1 ? 0 : 1; i < argc; i++)
    if (strcmp(argv[i], key) == 0) return 1;

  return 0;
}

int main(int argc, char* argv[])
{
  dump = 0;
  GC = init_gc();

  char* contents = fjb_read_file(argv[1]);
  gen_flags_T flags;
  flags.filepath = argv[1];
  dump = getarg(argc, argv, "-d");
  list_T* es_exports = init_list(sizeof(AST_T*));
  gc_mark_list(GC, es_exports);
  compiler_result_T* result = fjb(flags, contents, es_exports);
  
  printf("%s\n", dump ? result->dumped : result->stdout);

  gc_sweep(GC);
  gc_free(GC);

  compiler_result_free(result);
  free(contents);

  return 0;
}
