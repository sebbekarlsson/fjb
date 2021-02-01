#ifndef FJB_H
#define FJB_H
#include "env.h"
#include "result.h"

compiler_result_T* fjb();

char* fjb_get_headers(fjb_env_T* env);
#endif
