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
  init_fjb_env();

  cmd_opt_T opt = {};
  char* outfile = 0;
  opt = cmd_getflag(argc, argv, "--help", &opt);
  if (opt.key)
    return cmd_help(opt);

  opt = cmd_getflag(argc, argv, "--version", &opt);
  if (opt.key)
    return cmd_version(opt);

  opt = cmd_getarg(argc, argv, "-o", &opt);
  if (opt.value) {
    outfile = strdup(opt.value);
    fjb_set_outfile((char*)opt.value);
  } else {
    return cmd_help(opt);
  }

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

  opt = (cmd_opt_T){};
  opt = cmd_getflag(argc, argv, "--watch", &opt);
  if (opt.key)
    return cmd_watch(opt);

  compiler_result_T* result = fjb();

  if (!result || (result && !result->stdout)) {
    printf("Nothing to do.\n");
    return 0;
  }

  fjb_write_file(outfile, result->stdout);

  compiler_result_free(result);

  destroy_fjb_env();

  return 0;
}
