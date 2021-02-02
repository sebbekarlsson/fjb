#ifndef FJB_SPECIAL_GEN_H
#define FJB_SPECIAL_GEN_H
#include "env.h"
#include "result.h"
compiler_result_T* special_gen(fjb_env_T* env, char* ext, list_T* imports);

compiler_result_T* special_gen_json(fjb_env_T* env, list_T* imports);

compiler_result_T* special_gen_css(fjb_env_T* env, list_T* imports);
#endif
