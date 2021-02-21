#include "include/watch.h"
#include "include/colors.h"
#include "include/env.h"
#include "include/fjb.h"
#include "include/io.h"
#include "include/string_utils.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct DIRINFO
{
  char* dirname;
  char* path;
} dir_info_T;

extern fjb_env_T* FJB_ENV;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

static void list_dir(const char* dir_name, void (*callback)(char* path))
{
  DIR* d;

  /* Open the directory specified by "dir_name". */

  d = opendir(dir_name);

  /* Check it was opened. */
  if (!d) {
    fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  while (1) {
    struct dirent* entry;
    const char* d_name;

    /* "Readdir" gets subsequent entries from "d". */
    entry = readdir(d);
    if (!entry) {
      /* There are no more entries in this directory, so break
         out of the while loop. */
      break;
    }
    d_name = entry->d_name;
    /* Print the name of the file and directory. */

#if 0
    /* If you don't want to print the directories, use the
       following line: */

    if (! (entry->d_type & DT_DIR)) {
      printf ("%s/%s\n", dir_name, d_name);
    }

#endif /* 0 */

    if (entry->d_type & DT_DIR) {

      /* Check that the directory is not "d" or d's parent. */

      if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
        int path_length;
        char path[PATH_MAX];

        path_length = snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name);

        if (path_length >= PATH_MAX) {
          fprintf(stderr, "Path length has got too long.\n");
          exit(EXIT_FAILURE);
        }
        /* Recursively call "list_dir" with the new path. */
        list_dir(path, callback);
      }
    }
  }
  /* After going through all the entries, close the directory. */
  if (closedir(d)) {
    fprintf(stderr, "Could not close '%s': %s\n", dir_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  if (dir_name)
    callback((char*)dir_name);
}

static unsigned int should_emit(struct inotify_event* i)
{
  if (!i->name)
    return 0;

  const char* ext = get_filename_ext(strdup(i->name));

  if (!ext)
    return 0;

  if (!(strcmp(ext, ".js") == 0 || strcmp(ext, ".jsx") == 0 || strcmp(ext, ".ts") == 0 ||
        strcmp(ext, ".tsx") == 0 || strcmp(ext, ".css") == 0 || strcmp(ext, ".json") == 0))
    return 0;

  if (strstr(i->name, "dist.js"))
    return 0;
  if (strstr(i->name, "min.js"))
    return 0;
  if (strstr(i->name, "#"))
    return 0;

  if (i->mask & IN_CLOSE_WRITE)
    return 1;
  if (i->mask & IN_CREATE)
    return 1;
  if (i->mask & IN_DELETE)
    return 1;
  // if (i->mask & IN_DELETE_SELF)   return 1;
  if (i->mask & IN_ISDIR)
    return 1;
  // if (i->mask & IN_MODIFY)        return 1;
  // if (i->mask & IN_MOVE_SELF)     return 1;
  // if (i->mask & IN_MOVED_FROM)    return 1;
  // if (i->mask & IN_MOVED_TO)      return 1;

  return 0;
}

static void watch(char* dir, char* outfile)
{
  int inotifyFd, wd, j;
  char buf[BUF_LEN] __attribute__((aligned(8)));
  ssize_t numRead;
  char* p;
  struct inotify_event* event;

  inotifyFd = inotify_init(); /* Create inotify instance */
  if (inotifyFd == -1)
    printf("inotify_init\n");

  char* path = strdup(FJB_ENV->filepath);

  /* For each command-line argument, add a watch for all events */

  wd = inotify_add_watch(inotifyFd, dir, IN_ALL_EVENTS);
  if (wd == -1)
    printf("inotify_add_watch\n");

  printf(GREEN "Watching %s using wd %d\n" RESET, dir, wd);

  for (;;) { /* Read events forever */
    numRead = read(inotifyFd, buf, BUF_LEN);
    if (numRead == 0)
      printf(RED "read() from inotify fd returned 0!\n" RESET);

    if (numRead == -1)
      printf("read\n");

    /* Process all of the events in buffer returned by read() */

    for (p = buf; p < buf + numRead;) {
      event = (struct inotify_event*)p;
      if (should_emit(event)) {
        printf(BLUE "Detected change in `%s`\n" RESET, event->name);
        printf("Bundling...\n", event->name);
        init_fjb_env();
        fjb_set_filepath(path);
        char* source = fjb_read_file(path);
        fjb_set_source(source);

        compiler_result_T* result = fjb();

        if (result && result->stdout)
          fjb_write_file(outfile, result->stdout);
        destroy_fjb_env();
        printf(GREEN "Done.\n" RESET);
        break;
      }

      p += sizeof(struct inotify_event) + event->len;
    }
  }
}

static void* run(void* ptr)
{
  char* path = (char*)ptr;
  char* outfile = strdup(FJB_ENV->outfile);
  char* dir = dirname(path);

  watch(path, outfile);
}

pthread_t watch_thread[100];
int i;

void callback(char* path)
{

  if (pthread_create(&watch_thread[i], 0, run, strdup(path))) {
    printf("Error creating watch_thread.\n");
  }
  i++;
}

void fjb_watch()
{
  i = 0;
  char* dir = dirname(FJB_ENV->filepath);
  list_dir(dir, callback);

  while (1) {
  }
}
