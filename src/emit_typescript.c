#include "include/emit_typescript.h"
#include "include/emit.h"
#include "include/env.h"
#include "include/js/data_type.js.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

#define TEMPLATE(template_name, tmpname, len, ...)                                                 \
  tmpname = calloc(_tmp_##template_name##_js_len + len + 128, sizeof(char));                       \
  sprintf(tmpname, (const char*)_tmp_##template_name##_js, __VA_ARGS__);

char* emit_ts(AST_T* ast, fjb_env_T* env)
{
  char* body = 0;

  switch (ast->type) {
    case AST_INTERFACE: body = emit_interface(ast, env); break;
    case AST_DATA_TYPE: body = emit_data_type(ast, env); break;
    default: {
      printf("[Gen(Typescript)]: Missing emiterator for `%d`\n", ast->type);
      exit(1);
    } break;
  }

  return body;
}

char* emit_interface(AST_T* ast, fjb_env_T* env)
{
  return strdup("");
}

char* emit_data_type(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* name = ast->name ? strdup(ast->name) : strdup("");
  TEMPLATE(data_type, str, strlen(name), name);

  return str ? str : strdup("");
}
