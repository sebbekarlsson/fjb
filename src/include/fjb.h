#ifndef FJB_H
#define FJB_H
#include "gen.h"
#include "list.h"

typedef struct FJB_COMPILER_RESULT
{
  char* stdout;
  list_T* args;
  list_T* es_exports;
} compiler_result_T;

void compiler_result_free(compiler_result_T* result);

compiler_result_T* fjb(GEN_FLAGS flags, char *source, list_T* refs, list_T* imports, unsigned int all);
#endif
