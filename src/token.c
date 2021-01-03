#include "include/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


token_T* init_token(char* value, int type)
{
  token_T* token = calloc(1, sizeof(struct FJB_TOKEN_STRUCT));
  token->value = value;
  token->type = type;

  return token;
}

char* token_to_str(token_T* token)
{
  if (!token) return strdup("<token nil>");

  char* op = token->value ? token->value: strdup("(unknown)");
  const char* template = "<token value='%s' type='%s'>";
  char* typestr = token_type_to_str(token->type);
  char* value = calloc(strlen(template) + strlen(op) + strlen(typestr) + 32, sizeof(char));

  sprintf(value, template, op, typestr);

  return value;
}


char* token_type_to_str(int type)
{
  switch (type) {
    case TOKEN_ID: return strdup("TOKEN_ID");
    case TOKEN_LBRACE: return strdup("TOKEN_LBRACE");
    case TOKEN_RBRACE: return strdup("TOKEN_RBRACE");
    case TOKEN_LBRACKET: return strdup("TOKEN_LBRACKET");
    case TOKEN_RBRACKET: return strdup("TOKEN_RBRACKET");
    case TOKEN_SEMI: return strdup("TOKEN_SEMI");
    case TOKEN_LPAREN: return strdup("TOKEN_LPAREN");
    case TOKEN_RPAREN: return strdup("TOKEN_RPAREN");
    case TOKEN_STRING: return strdup("TOKEN_STRING");
    case TOKEN_REGEX: return strdup("TOKEN_REGEX");
    case TOKEN_INT: return strdup("TOKEN_INT");
    case TOKEN_FLOAT: return strdup("TOKEN_FLOAT");
    case TOKEN_EOF: return strdup("TOKEN_EOF");
    case TOKEN_EQUALS: return strdup("TOKEN_EQUALS");
    case TOKEN_EQUALS_EQUALS: return strdup("TOKEN_EQUALS_EQUALS");
    case TOKEN_EQUALS_EQUALS_EQUALS: return strdup("TOKEN_EQUALS_EQUALS_EQUALS");
    case TOKEN_NOT_EQUALS: return strdup("TOKEN_NOT_EQUALS");
    case TOKEN_NOT_EQUALS_EQUALS: return strdup("TOKEN_NOT_EQUALS_EQUALS");
    case TOKEN_PLUS_EQUALS: return strdup("TOKEN_PLUS_EQUALS");
    case TOKEN_STAR_EQUALS: return strdup("TOKEN_STAR_EQUALS");
    case TOKEN_MINUS_EQUALS: return strdup("TOKEN_");
    case TOKEN_INCREMENT: return strdup("TOKEN_INCREMENT");
    case TOKEN_DECREMENT: return strdup("TOKEN_DECREMENT");
    case TOKEN_LT: return strdup("TOKEN_LT");
    case TOKEN_LT_EQUALS: return strdup("TOKEN_LT_EQUALS");
    case TOKEN_GT: return strdup("TOKEN_GT");
    case TOKEN_GT_EQUALS: return strdup("TOKEN_GT_EQUALS");
    case TOKEN_ARROW_RIGHT: return strdup("TOKEN_ARROW_RIGHT");
    case TOKEN_SHIFT_RIGHT_UNSIGNED: return strdup("TOKEN_SHIFT_RIGHT_UNSIGNED");
    case TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS: return strdup("TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS");
    case TOKEN_PIPE_EQUALS: return strdup("TOKEN_PIPE_EQUALS");
    case TOKEN_AND_EQUALS: return strdup("TOKEN_AND_EQUALS");
    case TOKEN_TILDE: return strdup("TOKEN_TILDE");
    case TOKEN_DOT: return strdup("TOKEN_DOT");
    case TOKEN_COMMA: return strdup("TOKEN_COMMA");
    case TOKEN_COLON: return strdup("TOKEN_COLON");
    case TOKEN_INSTANCEOF: return strdup("TOKEN_INSTANCEOF");
    case TOKEN_AND: return strdup("TOKEN_AND");
    case TOKEN_AND_AND: return strdup("TOKEN_AND_AND");
    case TOKEN_NOT: return strdup("TOKEN_NOT");
    case TOKEN_WHILE: return strdup("TOKEN_WHILE");
    case TOKEN_SWITCH: return strdup("TOKEN_SWITCH");
    case TOKEN_BREAK: return strdup("TOKEN_BREAK");
    case TOKEN_DO: return strdup("TOKEN_DO");
    case TOKEN_IF: return strdup("TOKEN_IF");
    case TOKEN_ELSE: return strdup("TOKEN_ELSE");
    case TOKEN_FOR: return strdup("TOKEN_FOR");
    case TOKEN_FUNCTION: return strdup("TOKEN_FUNCTION");
    case TOKEN_TRY: return strdup("TOKEN_TRY");
    case TOKEN_CATCH: return strdup("TOKEN_CATCH");
    case TOKEN_THROW: return strdup("TOKEN_THROW");
    case TOKEN_RETURN: return strdup("TOKEN_RETURN");
    case TOKEN_DELETE: return strdup("TOKEN_DELETE");
    case TOKEN_NEW: return strdup("TOKEN_NEW");
    case TOKEN_IMPORT: return strdup("TOKEN_IMPORT");
    case TOKEN_EXPORT: return strdup("TOKEN_EXPORT");
    case TOKEN_FROM: return strdup("TOKEN_FROM");
    case TOKEN_AS: return strdup("TOKEN_AS");
    case TOKEN_TYPEOF: return strdup("TOKEN_TYPEOF");
    case TOKEN_ASYNC: return strdup("TOKEN_ASYNC");
    case TOKEN_AWAIT: return strdup("TOKEN_AWAIT");
    case TOKEN_ASSERT: return strdup("TOKEN_ASSERT");
    case TOKEN_CONST: return strdup("TOKEN_CONST");
    case TOKEN_LET: return strdup("TOKEN_LET");
    case TOKEN_VAR: return strdup("TOKEN_VAR");
    case TOKEN_PLUS: return strdup("TOKEN_PLUS");
    case TOKEN_IN: return strdup("TOKEN_IN");
    case TOKEN_STAR: return strdup("TOKEN_STAR");
    case TOKEN_MINUS: return strdup("TOKEN_MINUS");
    case TOKEN_DIV: return strdup("TOKEN_DIV");
    case TOKEN_MOD: return strdup("TOKEN_MOD");
    case TOKEN_PIPE: return strdup("TOKEN_PIPE");
    case TOKEN_PIPE_PIPE: return strdup("TOKEN_PIPE_PIPE");
    case TOKEN_QUESTION: return strdup("TOKEN_QUESTION");
    case TOKEN_ESCAPE: return strdup("TOKEN_ESCAPE");
    case TOKEN_HASH: return strdup("TOKEN_HASH");
    case TOKEN_SQUARED: return strdup("TOKEN_SQUARED");
    case TOKEN_DOLLAR: return strdup("TOKEN_DOLLAR");
    default: return strdup("TOKEN_UNSTRINGABLE");
  }
}

unsigned int token_is_statement_or_id(token_T* token)
{
  return token->type == TOKEN_ID || token->type == TOKEN_IMPORT || token->type == TOKEN_FOR || token->type == TOKEN_FROM;
}

void token_free(token_T* token)
{
  if (token->value)
    free(token->value);

  free(token);
}

token_T* token_clone(token_T* token)
{
  if (!token) return 0;

  token_T* new_token = init_token(strdup(token->value), token->type);
  return new_token;
}
