#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_T* init_token(char* value, int type)
{
  token_T* token = calloc(1, sizeof(struct FJB_TOKEN_STRUCT));
  token->value = value;
  token->type = type;

  return token;
}

char* token_to_str(token_T* token)
{
  if (!token)
    return strdup("<token nil>");

  char* op = token->value ? token->value : strdup("(unknown)");
  const char* template = "<token value='%s' type='%s'>";
  char* typestr = token_type_to_str(token->type);
  char* value = calloc(strlen(template) + strlen(op) + strlen(typestr) + 32, sizeof(char));

  sprintf(value, template, op, typestr);

  return value;
}

char* token_type_to_str(int type)
{
  return strdup(TOKEN_TYPE_STR[type]);
}

unsigned int token_is_statement_or_id(token_T* token)
{
  return token->type == TOKEN_ID || token->type == TOKEN_IMPORT || token->type == TOKEN_FOR ||
         token->type == TOKEN_FROM;
}

void token_free(token_T* token)
{
  if (token->value)
    free(token->value);

  free(token);
}

token_T* token_clone(token_T* token)
{
  if (!token)
    return 0;

  char* value = 0;

  if (token->value)
    value = strdup(token->value);
  else
    value = strdup("");

  token_T* new_token = init_token(value, token->type);
  new_token->c = token->c;
  return new_token;
}
