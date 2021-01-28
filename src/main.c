#include "include/fjb.h"
#include "include/io.h"
#include "include/signals.h"
#include <stdio.h>
#include <string.h>

unsigned int getarg(int argc, char* argv[], const char* key)
{
  for (int i = argc == 1 ? 0 : 1; i < argc; i++)
    if (strcmp(argv[i], key) == 0)
      return 1;

  return 0;
}

int main(int argc, char* argv[])
{
  init_fjb_signals();
  char* filepath = argv[1];
  char* source = fjb_read_file(filepath);
  unsigned int should_dump = getarg(argc, argv, "-d");

  compiler_flags_T* flags = init_compiler_flags(source, filepath, should_dump);

  compiler_result_T* result = fjb(flags);

  printf("%s\n", result->dumped ? result->dumped : result->stdout);

  gc_sweep(flags->GC);
  gc_free(flags->GC);

  compiler_result_free(result);

  return 0;
}
