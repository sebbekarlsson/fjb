#include "include/lexer.h"
#include "include/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) a > b ? b : a

lexer_T* init_lexer(char* source, const char* filepath)
{
  lexer_T* lexer = calloc(1, sizeof(struct FJB_LEXER_STRUCT));
  lexer->source = calloc(strlen(source) + 1, sizeof(char));
  strcpy(lexer->source, source);
  lexer->source_size = strlen(source);
  lexer->i = 0;
  lexer->c = lexer->source[lexer->i];
  lexer->cstr[0] = lexer->c;
  lexer->cstr[1] = '\0';
  lexer->line = 1;
  lexer->filepath = filepath;

  return lexer;
}

void lexer_advance(lexer_T* lexer)
{
  if (lexer->c == '\n') {
    lexer->line += 1;
  }

  if (lexer->c != '\0' && lexer->i < lexer->source_size) {
    lexer->i += 1;
    lexer->c = lexer->source[lexer->i];

    lexer->cstr[0] = lexer->c;
    lexer->cstr[1] = '\0';
  }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
  while ((lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\n' ||
          lexer->c == '\r') &&
         lexer->c != 0) {
    lexer_advance(lexer);
  }
}

void lexer_skip_comment(lexer_T* lexer)
{
  if (lexer->c == '/') {
    if (lexer_peek(lexer, 1) == '/') {
      while (lexer->c != '\n' && lexer->c != 0)
        lexer_advance(lexer);
    } else if (lexer_peek(lexer, 1) == '*') {
      while (1) {
        if (lexer->c == '*' && lexer_peek(lexer, 1) == '/') {
          lexer_advance(lexer);
          lexer_advance(lexer);
          break;
        }

        lexer_advance(lexer);

        if (lexer->c == 0)
          break;
      }
    }
  }

  lexer_skip_whitespace(lexer);
}

char lexer_peek(lexer_T* lexer, int offset)
{
  return lexer->source[MIN(lexer->i + offset, lexer->source_size - 1)];
}

unsigned int lexer_check_regex(lexer_T* lexer)
{
  if (!lexer->prev_token)
    return 0;

  if ((lexer->c == '/' && lexer_peek(lexer, 1) != '/' && lexer->prev_token->type != TOKEN_INT &&
       lexer->prev_token->type != TOKEN_HEX && lexer->prev_token->type != TOKEN_INT_MIN &&
       lexer->prev_token->type != TOKEN_DIV && lexer->prev_token->type != TOKEN_ID &&
       lexer->prev_token->type != TOKEN_RPAREN && lexer->prev_token->type != TOKEN_RBRACKET &&
       lexer->prev_token->type != TOKEN_LT && lexer->prev_token->type != TOKEN_RBRACE &&
       lexer->prev_token->type != TOKEN_STRING) ||
      (lexer->c == '/' && lexer_peek(lexer, 1) == '^')) {
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
    if (lexer->c == '\t' || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\n' ||
        lexer->c == '\r') {
      lexer_skip_whitespace(lexer);
    }

    if (lexer->c == '/') {
      while (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*') {
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        continue;
      }
    }

    if (lexer_check_regex(lexer)) {
      return lexer_parse_regex(lexer);
    }

    if (lexer->c == '0' && lexer_peek(lexer, 1) == 'x') {
      return lexer_parse_hex(lexer);
    }

    if (isdigit(lexer->c) || (lexer->c == '.' && isdigit(lexer_peek(lexer, 1)))) {
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
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("=>"), TOKEN_ARROW_RIGHT));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=') {
        token_T* tok =
          lexer_advance_token(lexer, init_token(strdup("==="), TOKEN_EQUALS_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      } else if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("=="), TOKEN_EQUALS_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '!') {
      if (lexer_peek(lexer, 1) == '=' && lexer_peek(lexer, 2) == '=') {
        token_T* tok =
          lexer_advance_token(lexer, init_token(strdup("!=="), TOKEN_NOT_EQUALS_EQUALS));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      } else if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("!="), TOKEN_NOT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '+') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("+="), TOKEN_PLUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '+') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("++"), TOKEN_INCREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '|') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("|="), TOKEN_PIPE_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '*') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("*="), TOKEN_STAR_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '.') {
      if (lexer_peek(lexer, 1) == '.' && lexer_peek(lexer, 2) == '.') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("..."), TOKEN_SPREAD));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '>') {
      if (lexer_peek(lexer, 1) == '>' && lexer_peek(lexer, 2) == '>' &&
          lexer_peek(lexer, 3) == '=') {
        token_T* tok =
          lexer_advance_token(lexer, init_token(strdup(">>>="), TOKEN_SHIFT_RIGHT_UNSIGNED_EQUALS));
        lexer_advance(lexer);
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (lexer_peek(lexer, 1) == '>' && lexer_peek(lexer, 2) == '=') {
        token_T* tok = lexer_advance_token(
          lexer, init_token(strdup(">>="), TOKEN_UNSIGNED_SHIFT_RIGHT_ASSIGNMENT));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (lexer_peek(lexer, 1) == '>' && lexer_peek(lexer, 2) == '>') {
        token_T* tok =
          lexer_advance_token(lexer, init_token(strdup(">>>"), TOKEN_BITWISE_SHIFT_RIGHT_UNSIGNED));
        lexer_advance(lexer);
        return lexer_advance_token(lexer, tok);
      }
      if (lexer_peek(lexer, 1) == '>') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup(">>"), TOKEN_SHIFT_RIGHT));
        lexer_advance(lexer);
        return tok;
      }
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup(">="), TOKEN_GT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '<') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup(">="), TOKEN_LT_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
      if (lexer_peek(lexer, 1) == '<') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("<<"), TOKEN_SHIFT_LEFT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '%') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("%="), TOKEN_MOD_EQUALS));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '-') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("-="), TOKEN_MINUS_EQUALS));
        lexer_advance(lexer);
        return tok;
      } else if (lexer_peek(lexer, 1) == '-') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("--"), TOKEN_DECREMENT));
        lexer_advance(lexer);
        return tok;
      }
    }

    if (lexer->c == '|') {
      if (lexer_peek(lexer, 1) == '|') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("||"), TOKEN_PIPE_PIPE));
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '&') {
      if (lexer_peek(lexer, 1) == '&') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("&&"), TOKEN_AND_AND));
        return lexer_advance_token(lexer, tok);
      }

      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("&="), TOKEN_AND_EQUALS));
        return lexer_advance_token(lexer, tok);
      }
    }

    if (lexer->c == '/') {
      if (lexer_peek(lexer, 1) == '=') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("/="), TOKEN_DIV_EQUALS));
        return lexer_advance_token(lexer, tok);
      }
      while (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*') {
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        continue;
      }
    }

    if (lexer->c == '?') {
      if (lexer_peek(lexer, 1) == '.') {
        token_T* tok = lexer_advance_token(lexer, init_token(strdup("?."), TOKEN_OPTIONAL_CHAIN));
        return lexer_advance_token(lexer, tok);
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
      printf("[Lexer]: Unexpected token (%s):%d: `%c` (%d)\n",
             lexer->filepath,
             lexer->line,
             lexer->c,
             (int)lexer->c);
      break;
    }
  }

  return ret_tok(lexer, init_token(charstr(lexer->c), TOKEN_EOF));
}

token_T* lexer_parse_id(lexer_T* lexer)
{
  char* str = strdup("");

  while (isalnum(lexer->c) || isdigit(lexer->c) || lexer->c == '_' || lexer->c == '$') {
    str = str_append(&str, lexer->cstr);
    lexer_advance(lexer);
  }

  return lexer_switch_id(lexer, init_token(str, TOKEN_ID));
}

token_T* lexer_parse_hex(lexer_T* lexer)
{
  char* str = 0;

  while (isalnum(lexer->c) || isdigit(lexer->c)) {
    str = str_append(&str, lexer->cstr);
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
    if (lexer->c == '\\') {
      str = str_append(&str, "\\");
      lexer_advance(lexer);
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
      continue;
    }
    if (lexer->c == start) {
      break;
    } else {
      if (lexer->c == '"' && start == '\'') {
        str = str_append(&str, "\\");
      }
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }
  }

  if (lexer->c == start)
    lexer_advance(lexer);

  token_T* token = init_token(str ? str : strdup(""), TOKEN_STRING);
  token->c = start;
  return ret_tok(lexer, token);
}

token_T* lexer_parse_regex(lexer_T* lexer)
{
  char* str = 0;
  char prevc;

  lexer_skip_whitespace(lexer);

  str = str_append(&str, lexer->cstr);
  prevc = lexer->c;
  lexer_advance(lexer);

  unsigned int is_bracket = 0;

  while (lexer->c != 0) {
    if (lexer->c == '\\' && lexer_peek(lexer, 1) == '\\') {
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);

      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
      continue;
    }

    if (lexer->c == '[' && prevc != '\\') {
      is_bracket = 1;
    } else if (lexer->c == ']' && prevc != '\\' && is_bracket) {
      is_bracket = 0;
    }

    if (lexer->c == '/' && prevc != '\\' && !is_bracket) {
      str = str_append(&str, lexer->cstr);
      prevc = lexer->c;
      lexer_advance(lexer);
      break;
    }

    str = str_append(&str, lexer->cstr);
    prevc = lexer->c;
    lexer_advance(lexer);
  }

  while (lexer->c == 'g' || lexer->c == 'i' || lexer->c == 'm') {
    str = str_append(&str, lexer->cstr);
    prevc = lexer->c;
    lexer_advance(lexer);
  }

  str = str ? str : strdup("");

  token_T* tok = ret_tok(lexer, init_token(str, TOKEN_REGEX));
  return tok;
}

token_T* lexer_parse_number(lexer_T* lexer)
{
  char* str = 0;
  int type = TOKEN_INT;

  if (lexer->c == '.') {
    str = str_append(&str, "0");
    str = str_append(&str, lexer->cstr);
    lexer_advance(lexer);
  }

  if (isdigit(lexer->c) && (lexer_peek(lexer, 1) == 'e' || lexer_peek(lexer, 1) == 'E')) {
    type = TOKEN_INT_MIN;

    while (isdigit(lexer->c) || lexer->c == 'e' || lexer->c == 'E') {
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }
  } else {
    while (isdigit(lexer->c) || lexer->c == 'e') {
      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);
    }

    if (lexer->c == '.') {
      type = TOKEN_FLOAT;

      str = str_append(&str, lexer->cstr);
      lexer_advance(lexer);

      while (isdigit(lexer->c) || lexer->c == 'e') {

        if (lexer->c == 'e' && lexer_peek(lexer, 1) == '+') {
          str = str_append(&str, lexer->cstr);
          str = str_append(&str, "+");
          lexer_advance(lexer);
          lexer_advance(lexer);

          continue;
        }

        str = str_append(&str, lexer->cstr);
        lexer_advance(lexer);
      }
    }
  }

  return ret_tok(lexer, init_token(str, type));
}

token_T* lexer_parse_any(lexer_T* lexer, char stop_char, char stop_char2)
{
  int type = TOKEN_RAW;

  char* value = 0;

  while (lexer->c != stop_char && lexer->c != stop_char2 && lexer->c != 0) {
    if (lexer->c == '{') {
      value = str_append(&value, "$");
      type = TOKEN_TEMPLATE_STRING;
    }

    value = str_append(&value, lexer->cstr);
    lexer_advance(lexer);
  }

  token_T* token = init_token(value, type);

  return ret_tok(lexer, token);
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
  else if (strcmp(token->value, "default") == 0)
    token->type = TOKEN_DEFAULT;
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
  else if (strcmp(token->value, "interface") == 0)
    token->type = TOKEN_INTERFACE;
  else if (strcmp(token->value, "try") == 0)
    token->type = TOKEN_TRY;
  else if (strcmp(token->value, "catch") == 0)
    token->type = TOKEN_CATCH;
  else if (strcmp(token->value, "finally") == 0)
    token->type = TOKEN_FINALLY;
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
  else if (strcmp(token->value, "class") == 0)
    token->type = TOKEN_CLASS;
  else if (strcmp(token->value, "extends") == 0)
    token->type = TOKEN_EXTENDS;
  else if (strcmp(token->value, "get") == 0)
    token->type = TOKEN_GET;

  return ret_tok(lexer, token);
}

token_T* ret_tok(lexer_T* lexer, token_T* token)
{
  if (lexer->prev_token)
    token_free(lexer->prev_token);

  lexer->prev_token = token_clone(token);
  return token;
}

void lexer_free(lexer_T* lexer)
{
  if (lexer->prev_token)
    token_free(lexer->prev_token);

  if (lexer->source)
    free(lexer->source);

  free(lexer);
}

lexer_T* lexer_copy(lexer_T* lexer)
{
  lexer_T* lex = init_lexer(lexer->source, lexer->filepath);
  lex->c = lexer->c;
  lex->i = lexer->i;
  lex->line = lexer->line;
  lex->prev_token = token_clone(lexer->prev_token);
  memcpy(lex->cstr, lexer->cstr, sizeof(lexer->cstr));

  return lex;
}

token_T* lexer_peek_next_token(lexer_T* lexer)
{
  lexer_T* copy = lexer_copy(lexer);
  token_T* next_token = lexer_next(copy);
  lexer_free(copy);

  return next_token;
}
