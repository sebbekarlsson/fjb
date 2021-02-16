#ifndef FJB_CMD_H
#define FJB_CMD_H
typedef struct CMD_OPT_STRUCT
{
  const char* key;
  const char* value;
} cmd_opt_T;

cmd_opt_T cmd_getopt(int argc, char* argv[], const char* key, cmd_opt_T* opt);
cmd_opt_T cmd_getflag(int argc, char* argv[], const char* key, cmd_opt_T* opt);
int cmd_help();
int cmd_version();
#endif
