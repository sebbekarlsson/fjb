#ifndef FJB_TOKEN_H
#define FJB_TOKEN_H
#include "enums/TOKEN_TYPES.gpp.h"
typedef struct FJB_TOKEN_STRUCT
{
  char* value;
  enum TOKEN_TYPE type;
  char c;
} token_T;

token_T* init_token(char* value, int type);

char* token_to_str(token_T* token);

char* token_type_to_str(int type);

void token_free(token_T* token);

token_T* token_clone(token_T* token);
#endif
