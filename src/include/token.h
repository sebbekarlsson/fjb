#ifndef FJB_TOKEN_H
#define FJB_TOKEN_H
typedef struct FJB_TOKEN_STRUCT {
  char* value;
  enum {
    TOKEN_ID,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_EOF,
    TOKEN_EQUALS,
    TOKEN_EQUALS_EQUALS,
    TOKEN_EQUALS_EQUALS_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_NOT_EQUALS_EQUALS,
    TOKEN_PLUS_EQUALS,
    TOKEN_STAR_EQUALS,
    TOKEN_MINUS_EQUALS,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_LT,
    TOKEN_LT_EQUALS,
    TOKEN_GT,
    TOKEN_GT_EQUALS,
    TOKEN_ARROW_RIGHT,
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_INSTANCEOF,
    TOKEN_AND,
    TOKEN_AND_AND,
    TOKEN_NOT,
    TOKEN_WHILE,
    TOKEN_SWITCH,
    TOKEN_BREAK,
    TOKEN_DO,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_FUNCTION,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_THROW,
    TOKEN_RETURN,
    TOKEN_NEW,
    TOKEN_IMPORT,
    TOKEN_EXPORT,
    TOKEN_FROM,
    TOKEN_AS,
    TOKEN_TYPEOF,
    TOKEN_CONST,
    TOKEN_LET,
    TOKEN_VAR,
    TOKEN_PLUS,
    TOKEN_IN,
    TOKEN_STAR,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_PIPE,
    TOKEN_PIPE_PIPE,
    TOKEN_QUESTION,
  } type;
} token_T;

token_T* init_token(char* value, int type);

char* token_to_str(token_T* token);
#endif
