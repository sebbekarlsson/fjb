#ifndef FJB_H
#define FJB_H
#include "AST.h"
#include "flags.h"
#include "list.h"

typedef struct FJB_COMPILER_RESULT
{
  char* stdout;
  char* dumped;
  char* filepath;
  list_T* es_exports;
  AST_T* node;
} compiler_result_T;

void compiler_result_free(compiler_result_T* result);

compiler_result_T* fjb(compiler_flags_T* flags);
#endif
