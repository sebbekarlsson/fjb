#ifndef FJB_RESULT_H
#define FJB_RESULT_H
#include "AST.h"
#include "list.h"
typedef struct FJB_COMPILER_RESULT
{
  char* stdout;
  char* dumped;
  char* filepath;
  list_T* es_exports;
  AST_T* node;
} compiler_result_T;

compiler_result_T* init_compiler_result(char* stdout, char* filepath);

void compiler_result_free(compiler_result_T* result);
#endif
