#ifndef FJB_SPECIAL_GEN_H
#define FJB_SPECIAL_GEN_H
#include "env.h"
#include "result.h"
compiler_result_T* emit_hooks(char* ext, list_T* imports);

compiler_result_T* emit_hooks_json(list_T* imports);

compiler_result_T* emit_hooks_css(list_T* imports);
#endif
