#ifndef FJB_LEXER_H
#define FJB_LEXER_H
#include "token.h"

typedef struct FJB_LEXER_STRUCT {
  char* source;
  unsigned int source_size;
  char c;
  unsigned int i;
  int line;
} lexer_T;

lexer_T* init_lexer(char* source);

void lexer_advance(lexer_T* lexer);

void lexer_skip_whitespace(lexer_T* lexer);

void lexer_skip_comment(lexer_T* lexer);

char lexer_peek(lexer_T* lexer, int offset);

token_T* lexer_advance_token(lexer_T* lexer, token_T* token);

token_T* lexer_next(lexer_T* lexer);

token_T* lexer_parse_id(lexer_T* lexer);

token_T* lexer_parse_string(lexer_T* lexer);

token_T* lexer_parse_number(lexer_T* lexer);

token_T* lexer_switch_id(lexer_T* lexer, token_T* token);

#endif
