#include "include/result.h"
#include <string.h>

compiler_result_T* init_compiler_result(char* stdout, char* filepath)
{
  compiler_result_T* result = calloc(1, sizeof(compiler_result_T));
  result->stdout = strdup(stdout);
  result->filepath = strdup(filepath);

  return result;
}

void compiler_result_free(compiler_result_T* result)
{
  if (!result)
    return;

  if (result->stdout)
    free(result->stdout);

  if (result->headers)
    free(result->headers);

  free(result);
}
