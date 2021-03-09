#include "include/id.h"
#include "include/constants.h"
#include "include/env.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

const char alpha[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                       'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

extern fjb_env_T* FJB_ENV;

void id_generate(AST_T* ast)
{
  if (ast->alias)
    return;

  char* source = ast->name;

  if (!source || !FJB_ENV)
    return;

  if (str_is_upper(source))
    return;

  unsigned int len = strlen(source);
  if (len <= 3)
    return;
  if (strchr(source, '$'))
    return;

  unsigned int alpha_len = sizeof(alpha) / sizeof(alpha[0]);
  char* v = strdup(source);
  v[MAX(0, len - 1)] = alpha[len % alpha_len];

  ast->alias = strdup(v);
}
