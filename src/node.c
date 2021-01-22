#include "include/node.h"
#include <string.h>

const char* NODE_MODULES[] = {
  "assert",      "async_hooks",  "buffer", "child_process", "cluster",        "console",
  "constants",   "crypto",       "dgram",  "dns",           "domain",         "events",
  "fs",          "http",         "http2",  "https",         "inspector",      "module",
  "net",         "os",           "path",   "perf_hooks",    "process",        "punycode",
  "querystring", "readline",     "repl",   "stream",        "string_decoder", "timers",
  "tls",         "trace_events", "tty",    "url",           "util",           "v8",
  "vm",          "zlib"
};

unsigned int is_builtin_module(char* name)
{
  for (int i = 0; i < NODE_MODULES_LENGTH; i++) {
    if (strcmp(NODE_MODULES[i], name) == 0)
      return 1;
  }

  return 0;
}
