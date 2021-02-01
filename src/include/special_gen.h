#ifndef FJB_SPECIAL_GEN_H
#define FJB_SPECIAL_GEN_H
#include "env.h"
#include "result.h"
compiler_result_T* special_gen(fjb_env_T* env, char* ext);

compiler_result_T* special_gen_json(fjb_env_T* env);

compiler_result_T* special_gen_css(fjb_env_T* env);
#endif
