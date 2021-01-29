#ifndef FJB_SPECIAL_GEN_H
#define FJB_SPECIAL_GEN_H
#include "flags.h"
#include "result.h"
compiler_result_T* special_gen(compiler_flags_T* flags, char* ext);

compiler_result_T* special_gen_json(compiler_flags_T* flags);

compiler_result_T* special_gen_css(compiler_flags_T* flags);
#endif
