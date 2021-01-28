#ifndef FJB_H
#define FJB_H
#include "flags.h"
#include "result.h"

compiler_result_T* fjb(compiler_flags_T* flags);

char* fjb_get_headers(compiler_flags_T* flags);
#endif
