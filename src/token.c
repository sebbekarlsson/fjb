#include "include/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


token_T* init_token(char* value, int type)
{
  token_T* token = calloc(1, sizeof(struct FJB_TOKEN_STRUCT));
  if (value) {
    token->value =  calloc(strlen(value) + 1, sizeof(char));
    strcpy(token->value, value);
  } else {
    token->value = 0;
  }
  token->type = type;

  return token;
}

char* token_to_str(token_T* token)
{
  if (!token) return strdup("<token nil>");

  char* op = token->value ? token->value: strdup("(unknown)");
  const char* template = "<token value='%s' type='%d'>";
  char* value = calloc(strlen(template) + strlen(op) + 32, sizeof(char));

  sprintf(value, template, op, token->type);

  return value;
}
