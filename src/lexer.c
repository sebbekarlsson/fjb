#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"
#include "include/string_utils.h"
#include <stdio.h>

#define MIN(a, b)\
  a > b ? b : a

lexer_T* init_lexer(char* source, const char* filepath)
{
  lexer_T* lexer = calloc(1, sizeof(struct FJB_LEXER_STRUCT));
  lexer->source = calloc(strlen(source) + 1, sizeof(char));
  strcpy(lexer->source, source);
  lexer->source_size = strlen(source);
  lexer->i = 0;
  lexer->c = lexer->source[lexer->i];
  lexer->line = 1;
  lexer->filepath = filepath;

  return lexer;
}

void lexer_advance(lexer_T* lexer)
{
  if (lexer->c != '\0' && lexer->i < lexer->source_size) {
    lexer->i += 1;
    lexer->c = lexer->source[lexer->i];

    if (lexer->c == '\n')
    {
      lexer->line += 1;
    }
  }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
  while ((lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\n' || lexer->c == '\r') && lexer->c != 0) {
    lexer_advance(lexer);
  }
}

void lexer_skip_comment(lexer_T* lexer)
{
  if (lexer->c == '/')
  {
    if (lexer_peek(lexer, 1) == '/') 
    {
      while (lexer->c != '\n')
        lexer_advance(lexer);
    }
    else
    if (lexer_peek(lexer, 1) == '*')
    {
      while (1) {
        if (lexer->c == '*' && lexer_peek(lexer, 1) == '/')
        {
          lexer_advance(lexer);
          lexer_advance(lexer);
          break;
        }

        lexer_advance(lexer);
      }
    }
  }

  lexer_skip_whitespace(lexer);
}

char lexer_peek(lexer_T* lexer, int offset)
{
  return lexer->source[MIN(lexer->i + offset, lexer->source_size-1)];
}

unsigned int lexer_check_regex(lexer_T* lexer)
{
  if (!lexer->prev_token) return 0;

  if (
    lexer->c == '/' &&
    lexer_peek(lexer, 1) != '/' &&
    lexer->prev_token->type != TOKEN_INT &&
    lexer->prev_token->type != TOKEN_DIV &&
    lexer->prev_token->type != TOKEN_ID &&
    lexer->prev_token->type != TOKEN_RPAREN &&
    lexer->prev_token->type != TOKEN_RBRACE &&
    lexer->prev_token->type != TOKEN_STRING
  )
  {
    return 1;
  }

  return 0;
}

token_T* lexer_advance_token(lexer_T* lexer, token_T* token)
{
  lexer_advance(lexer);
  return ret_tok(lexer, token);
}

token_T* lexer_next(lexer_T* lexer)
{
  while (lexer->c != 0) {
    if (lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\n' || lexer->c == '\r') {
      lexer_skip_whitespace(lexer);
    }

    if (lexer->c == '/') {
      while (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*')
      {
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        continue;
      }
    }

    if (lexer_check_regex(lexer))
    {
      return lexer_parse_regex(lexer);
    }

    if (lexer->c == '0' && lexer_peek(lexer, 1) == 'x')
    {
      return lexer_parse_hex(lexer);
    }

    if (isdigit(lexer->c)) {
      return lexer_parse_number(lexer);
    }

    if (isalnum(lexer->c) || lexer->c == '_' || lexer->c == '$') {
       return lexer_parse_id(lexer); 
    } 
    
    if (lexer->c == '"' || lexer->c == '\'' || lexer->c == '`') {
      return lexer_parse_string(lexer);
    }

    if (lexer->c == '=') {
      if (lexer_peek(lexer, 1) == '>') {
        token_T* tok = lexer_advance_token(lexer, init_token("=>", TOKEN_ARROW_RIGHT));
        lexer_advance(lexer);
        return tok;
      }
      else if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=' ) {
        token_T* tok = lexer_advance_token(lexer, init_token("===", TOKEN_EQUALS_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      else if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("==", TOKEN_EQUALS_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '!') {
      if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=' ) {
        token_T* tok = lexer_advance_token(lexer, init_token("!==", TOKEN_NOT_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      else if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("!=", TOKEN_NOT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '+') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("+=", TOKEN_PLUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
      else if (lexer_peek(lexer, 1) == '+') {
        token_T* tok = lexer_advance_token(lexer, init_token("++", TOKEN_INCREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '|') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("|=", TOKEN_PIPE_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '*') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("*=", TOKEN_STAR_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '.') {
      if (lexer_peek(lexer, 1) == '.' && lexer_peek(lexer, 2) == '.') {
        token_T* tok = lexer_advance_token(lexer, init_token("...", TOKEN_SPREAD));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '>') {
      if (lexer_peek(lexer, 1) == '>' && lexer_peek(lexer, 2) == '>' && lexer_peek(lexer, 3) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(">>>=", TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS));
        lexer_advance(lexer);
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (lexer_peek(lexer, 1) == '>' && lexer_peek(lexer, 2) == '>') {
        token_T* tok = lexer_advance_token(lexer, init_token(">>>", TOKEN_SHIFT_RIGHT_UNSIGNED));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(">=", TOKEN_GT_EQUALS));
        lexer_advance(lexer);
        return tok;
      } 
    }

    if (lexer->c == '<') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(">=", TOKEN_LT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '-') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("-=", TOKEN_MINUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
      else if (lexer_peek(lexer, 1) == '-') {
        token_T* tok = lexer_advance_token(lexer, init_token("--", TOKEN_DECREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '|') {
      if (lexer_peek(lexer, 1) == '|') {
        token_T* tok = lexer_advance_token(lexer, init_token("||", TOKEN_PIPE_PIPE));
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '&') {
      if (lexer_peek(lexer, 1) == '&') {
        token_T* tok = lexer_advance_token(lexer, init_token("&&", TOKEN_AND_AND)); 
        return lexer_advance_token(lexer, tok);
      }

      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token("&=", TOKEN_AND_EQUALS)); 
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '/') {
      while (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*')
      {
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        continue;
      }
    }

    switch (lexer->c) {
      case '{': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_LBRACE));
      case '}': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_RBRACE)); 
      case '(': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_LPAREN)); 
      case ')': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_RPAREN)); 
      case '[': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_LBRACKET)); 
      case ']': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_RBRACKET)); 
      case ';': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_SEMI)); 
      case '=': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_EQUALS)); 
      case '<': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_LT)); 
      case '>': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_GT)); 
      case '.': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_DOT)); 
      case ',': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_COMMA)); 
      case ':': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_COLON)); 
      case '!': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_NOT)); 
      case '&': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_AND)); 
      case '+': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_PLUS)); 
      case '-': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_MINUS)); 
      case '/': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_DIV)); 
      case '%': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_MOD));
      case '|': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_PIPE));
      case '*': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_STAR));
      case '?': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_QUESTION));
      case '\\': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_ESCAPE));
      case '#': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_HASH));
      case '^': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_SQUARED));
      case '~': return lexer_advance_token(lexer, init_token(charstr(lexer->c), TOKEN_TILDE));
      case '\0': break; 
    }

    if (lexer->c != 0) {
      printf("[Lexer]: Unexpected token (%s):%d: `%c` (%d)\n", lexer->filepath, lexer->line, lexer->c, (int) lexer->c);
      break;
    }
  }

  return ret_tok(lexer, init_token(charstr(lexer->c), TOKEN_EOF));
}

token_T* lexer_parse_id(lexer_T* lexer)
{
  char* str = strdup("");
  
  while (isalnum(lexer->c) || isdigit(lexer->c) || lexer->c == '_' || lexer->c == '$') {
    char* piece = charstr(lexer->c);
    str = str_append(&str, piece);
    lexer_advance(lexer);
  }

  return lexer_switch_id(lexer, init_token(str, TOKEN_ID));
}

token_T* lexer_parse_hex(lexer_T* lexer)
{
  char* str = strdup("");
  
  while (isalnum(lexer->c) || isdigit(lexer->c)) {
    char* piece = charstr(lexer->c);
    str = str_append(&str, piece);
    lexer_advance(lexer);
  }

  return lexer_switch_id(lexer, init_token(str, TOKEN_HEX));
}

token_T* lexer_parse_string(lexer_T* lexer)
{
  char* str = 0;
  char start = lexer->c;
  
  lexer_advance(lexer);

  while (lexer->c != 0 && lexer->c != start) { 
    if (lexer->c == start)
    {
      break;
    } else {
      if (lexer->c == '"' && start == '\'')
      {
        str = str_append(&str, "\\");
      }
      str = str_append(&str, charstr(lexer->c));
      lexer_advance(lexer);
    }
  }
 
  if (lexer->c == start) 
    lexer_advance(lexer);

  return ret_tok(lexer, init_token(str ? str : strdup(""), TOKEN_STRING));
}

token_T* lexer_parse_regex(lexer_T* lexer)
{
  char* str = 0;

  char prevc;

  lexer_skip_whitespace(lexer);

  str = str_append(&str, charstr(lexer->c));
  prevc = lexer->c;
  lexer_advance(lexer);

  while (lexer->c != 0) {
    if (lexer->c == '/' && prevc != '\\')
    {
      str = str_append(&str, charstr(lexer->c));
      prevc = lexer->c;
      lexer_advance(lexer);
      break;
    }

    str = str_append(&str, charstr(lexer->c));
    prevc = lexer->c;
    lexer_advance(lexer); 
  }

  if (lexer->c == 'g' || lexer->c == 'i')
  {
    str = str_append(&str, charstr(lexer->c));
    prevc = lexer->c;
    lexer_advance(lexer);
  }

  return ret_tok(lexer, init_token(str ? str : strdup(""), TOKEN_REGEX));
}

token_T* lexer_parse_number(lexer_T* lexer)
{
  char* str = 0;
  int type = TOKEN_INT;

  while (isdigit(lexer->c) || lexer->c == 'e') {
    char* piece = charstr(lexer->c);
    str = str_append(&str, piece);
    lexer_advance(lexer);
  }

  if (lexer->c == '.')
  {
    type = TOKEN_FLOAT;

    lexer_advance(lexer);

    while (isdigit(lexer->c) || lexer->c == 'e') {

      if (lexer->c == 'e' && lexer_peek(lexer, 1) == '+')
      {
        char* piece = charstr(lexer->c);
        str = str_append(&str, piece);
        str = str_append(&str, "+");
        lexer_advance(lexer);
        lexer_advance(lexer);

        continue;
      }

      char* piece = charstr(lexer->c);
      str = str_append(&str, piece);
      lexer_advance(lexer);
    }
  }

  return ret_tok(lexer, init_token(str, type));
}

token_T* lexer_switch_id(lexer_T* lexer, token_T* token)
{
  if (strcmp(token->value, "if") == 0)
    token->type = TOKEN_IF;
  else if (strcmp(token->value, "else") == 0)
    token->type = TOKEN_ELSE;
  else if (strcmp(token->value, "while") == 0)
    token->type = TOKEN_WHILE;
  else if (strcmp(token->value, "for") == 0)
    token->type = TOKEN_FOR;
  else if (strcmp(token->value, "return") == 0)
    token->type = TOKEN_RETURN;
  else if (strcmp(token->value, "new") == 0)
    token->type = TOKEN_NEW;
  else if (strcmp(token->value, "import") == 0)
    token->type = TOKEN_IMPORT;
  else if (strcmp(token->value, "export") == 0)
    token->type = TOKEN_EXPORT;
  else if (strcmp(token->value, "from") == 0)
    token->type = TOKEN_FROM;
  else if (strcmp(token->value, "as") == 0)
    token->type = TOKEN_AS;
  else if (strcmp(token->value, "const") == 0)
    token->type = TOKEN_CONST;
  else if (strcmp(token->value, "let") == 0)
    token->type = TOKEN_LET;
  else if (strcmp(token->value, "var") == 0)
    token->type = TOKEN_VAR;
  else if (strcmp(token->value, "function") == 0)
    token->type = TOKEN_FUNCTION;
  else if (strcmp(token->value, "try") == 0)
    token->type = TOKEN_TRY;
  else if (strcmp(token->value, "catch") == 0)
    token->type = TOKEN_CATCH;
  else if (strcmp(token->value, "finally") == 0)
    token->type = TOKEN_CATCH;
  else if (strcmp(token->value, "throw") == 0)
    token->type = TOKEN_THROW;
  else if (strcmp(token->value, "typeof") == 0)
    token->type = TOKEN_TYPEOF;
  else if (strcmp(token->value, "in") == 0)
    token->type = TOKEN_IN;
  else if (strcmp(token->value, "do") == 0)
    token->type = TOKEN_DO;
  else if (strcmp(token->value, "switch") == 0)
    token->type = TOKEN_SWITCH;
  else if (strcmp(token->value, "case") == 0)
    token->type = TOKEN_CASE;
  else if (strcmp(token->value, "break") == 0)
    token->type = TOKEN_BREAK;
  else if (strcmp(token->value, "instanceof") == 0)
    token->type = TOKEN_INSTANCEOF;
  else if (strcmp(token->value, "void") == 0)
    token->type = TOKEN_VOID;
  else if (strcmp(token->value, "async") == 0)
    token->type = TOKEN_ASYNC;
  else if (strcmp(token->value, "await") == 0)
    token->type = TOKEN_AWAIT;
  else if (strcmp(token->value, "assert") == 0)
    token->type = TOKEN_ASSERT;
  else if (strcmp(token->value, "delete") == 0)
    token->type = TOKEN_DELETE;

  return ret_tok(lexer, token);
}

token_T* ret_tok(lexer_T* lexer, token_T* token)
{
  lexer->prev_token = token;
  return token;
}
