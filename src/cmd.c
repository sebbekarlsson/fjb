#include "include/cmd.h"
#include "include/fjb.h"
#include "include/package.h"
#include "include/version.h"
#include <stdio.h>
#include <string.h>

cmd_opt_T cmd_getarg(int argc, char* argv[], const char* key, cmd_opt_T* opt)
{
  cmd_opt_T o = *opt;

  for (int i = argc == 1 ? 0 : 1; i < argc; i++) {
    if (strcmp(argv[i], key) == 0) {
      if (argc > i) {
        o.key = key;
        o.value = argv[i + 1];
        return o;
      }
    }
  }

  return o;
}

cmd_opt_T cmd_getflag(int argc, char* argv[], const char* key, cmd_opt_T* opt)
{
  cmd_opt_T o = *opt;

  for (int i = argc == 1 ? 0 : 1; i < argc; i++) {
    if (strcmp(argv[i], key) == 0) {
      o.key = key;
      if (argc > i) {
        o.value = argv[i + 1];
      }
      return o;
    }
  }

  return o;
}

int cmd_help()
{
  printf("Usage:\n"
         "\tfjb <input-file>\n");
  printf("--------\n");
  printf("Other flags:\n");
  printf("\tfjb --help\n");
  printf("\tfjb --version\n");
  return 0;
}

int cmd_version()
{
  printf("%s\n", _tmp_version_h);
  return 0;
}
