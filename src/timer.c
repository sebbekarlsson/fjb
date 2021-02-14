#include "include/timer.h"
#include "include/env.h"
#include "include/lexer.h"
#include "include/parser.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pthread_t timer_thread;

// the time we allow the compilation to run.
const int TIME_LIMIT_SECONDS = 4;

extern fjb_env_T* FJB_ENV;

/**
 * The purpose of this:
 *  - Strictly prohibit long compilation times
 *  - Provide an easy way to see where the bundler got stuck
 *    (if there's a parsing bug etc)
 */
void* timer_thread_run(void* ptr)
{
  time_t start, current;
  double diff_t;

  time(&start);

  while (1) {
    time(&current);
    diff_t = difftime(current, start);

    if (diff_t >= 4) {
      printf("Took too long. (probably stuck)\n");

      if (FJB_ENV->lexer) {
        printf("State at exit:\n");
        printf("\tFilepath: %s\n", FJB_ENV->lexer->filepath);
        printf("\tLexer->c = `%c` (%d)\n", FJB_ENV->lexer->c, FJB_ENV->lexer->c);
        printf("\tLexer->line = %d\n", FJB_ENV->lexer->line);
        printf("\tLexer->x = %d\n", FJB_ENV->lexer->x);
      }

      if (FJB_ENV->parser) {
        if (FJB_ENV->parser->token) {
          printf("\tParser->token = %s\n", token_to_str(FJB_ENV->parser->token));
        }
      }

      pthread_cancel(timer_thread);
      exit(1);
    }
  }

  return 0;
}

void timer_thread_start()
{
  if (timer_thread)
    return;

  if (pthread_create(&timer_thread, 0, timer_thread_run, 0)) {
    printf("Error creating timer_thread.\n");
    /**
     * We dont necessarily need to die here.
     */
  }
}
