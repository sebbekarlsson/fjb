#include "include/plugin.h"
#include "include/io.h"
#include "include/string_utils.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

extern fjb_env_T* FJB_ENV;

const char PLUGIN_DIR[] = ".plugins/";

void register_plugin_hook(plugin_hook hook)
{
  list_push(FJB_ENV->hooks, hook);
}

void* call_plugin_hook(plugin_hook hook, int action, void* ptr, fjb_env_T* env)
{
  return hook(action, ptr, env);
}

void load_hooks_from_directory(const char* directory)
{
  if (!directory)
    return;

  if (!file_exists((char*)directory))
    return;

  struct dirent* dp;
  DIR* dfd;

  if ((dfd = opendir(directory)) == NULL) {
    fprintf(stderr, "Can't open %s\n", directory);
    return;
  }

  char filename_qfd[360];

  while ((dp = readdir(dfd)) != NULL) {
    struct stat stbuf;
    if (!dp)
      continue;

    sprintf(filename_qfd, "%s/%s", directory, dp->d_name);

    if (stat(filename_qfd, &stbuf) == -1) {
      continue;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
      continue;
    } else {
      const char* ext = get_filename_ext(dp->d_name);
      if (strcmp(ext, ".so") != 0)
        continue;

      char buff[256];
      sprintf(buff, "%s%s", directory, dp->d_name);

      void* handle = dlopen(buff, RTLD_NOW);

      if (handle) {
        plugin_hook hook = dlsym(handle, "hook");
        register_plugin_hook(hook);
      } else {
        printf("Failed to load plugin `%s`\n", buff);
      }
    }
  }

  free(dfd);
}

void load_plugins()
{
  load_hooks_from_directory(PLUGIN_DIR);
}
