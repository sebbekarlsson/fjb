#include "include/cmd.h"
#include "include/env.h"
#include "include/fjb.h"
#include "include/io.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
  cmd_opt_T opt = {};
  opt = cmd_getflag(argc, argv, "--help", &opt);
  if (opt.key)
    return cmd_help(opt);

  opt = cmd_getflag(argc, argv, "--version", &opt);
  if (opt.key)
    return cmd_version(opt);

  init_fjb_env();
  load_plugins();

  char* filepath = argv[1];

  if (!filepath) {
    printf("You have not told me what to do.\n");
    printf("--------\n");
    cmd_help();
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
