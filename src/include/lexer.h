#ifndef FJB_LEXER_H
#define FJB_LEXER_H
#include "token.h"

typedef struct FJB_LEXER_STRUCT
{
  char* source;
  unsigned int source_size;
  char c;
  char cstr[2];
  unsigned int i;
  int line;
  const char* filepath;
  token_T* prev_token;
} lexer_T;

lexer_T* init_lexer(char* source, const char* filepath);

void lexer_advance(lexer_T* lexer);

void lexer_skip_whitespace(lexer_T* lexer);

void lexer_skip_comment(lexer_T* lexer);

char lexer_peek(lexer_T* lexer, int offset);

unsigned int lexer_check_regex(lexer_T* lexer);

token_T* lexer_advance_token(lexer_T* lexer, token_T* token);

token_T* lexer_next(lexer_T* lexer);

token_T* lexer_parse_id(lexer_T* lexer);

token_T* lexer_parse_hex(lexer_T* lexer);

token_T* lexer_parse_string(lexer_T* lexer);

token_T* lexer_parse_regex(lexer_T* lexer);

token_T* lexer_parse_number(lexer_T* lexer);

token_T* lexer_parse_any(lexer_T* lexer, char stop_token);

token_T* lexer_switch_id(lexer_T* lexer, token_T* token);

token_T* ret_tok(lexer_T* lexer, token_T* token);

void lexer_free(lexer_T* lexer);

#endif
