#include "include/env.h"
#include "include/fjb.h"
#include "include/io.h"
#include "include/plugin.h"
#include "include/string_utils.h"
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
  init_fjb_env();
  load_plugins();

  char* filepath = argv[1];
  char* source = fjb_read_file(filepath);
  unsigned int should_dump = getarg(argc, argv, "-d");

  fjb_set_source(source);
  fjb_set_filepath(filepath);

  compiler_result_T* result = fjb();

  printf("%s\n", result->stdout);

  compiler_result_free(result);

  destroy_fjb_env();

  return 0;
}
