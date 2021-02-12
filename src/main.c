#include "include/env.h"
#include "include/fjb.h"
#include "include/io.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

/*unsigned int getarg(int argc, char* argv[], const char* key)
{
  for (int i = argc == 1 ? 0 : 1; i < argc; i++)
    if (strcmp(argv[i], key) == 0)
      return 1;

  return 0;
}*/

void show_help()
{
  printf("Usage:\n"
         "\tfjb <input-file>\n");
}

int main(int argc, char* argv[])
{
  init_fjb_env();
  load_plugins();

  char* filepath = argv[1];

  if (!filepath) {
    printf("You have not told me what to do.\n");
    show_help();
    return 0;
  }

  char* source = fjb_read_file(filepath);

  fjb_set_source(source);
  fjb_set_filepath(filepath);

  compiler_result_T* result = fjb();

  if (!result || (result && !result->stdout)) {
    printf("Nothing to do.\n");
    return 0;
  }

  printf("%s\n", result->stdout);

  compiler_result_free(result);

  destroy_fjb_env();

  return 0;
}
