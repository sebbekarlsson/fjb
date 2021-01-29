#include "include/resolve.h"
#include <string.h>

AST_T* resolve(AST_T* ast, unsigned int (*query)(AST_T* ast, query_T data), query_T data)
{
  if (!ast)
    return 0;
  if (query(ast, data))
    return ast;

  AST_T* x = 0;

  if ((x = resolve(ast->left, query, data)) != 0)
    return x;
  // This shouldn't be needed... (I think)
  if ((x = resolve(ast->right, query, data)) != 0)
    return x;
  if ((x = resolve(ast->body, query, data)) != 0)
    return x;
  if ((x = resolve(ast->expr, query, data)) != 0)
    return x;
  if ((x = resolve(ast->value, query, data)) != 0)
    return x;
  if ((x = resolve(ast->next, query, data)) != 0)
    return x;
  if ((x = resolve(ast->condition, query, data)) != 0)
    return x;

  if (ast->list_value &&
      (ast->type == AST_COMPOUND || ast->type == AST_SCOPE || ast->type == AST_JSX_COMPOUND)) {
    LOOP_NODES(ast->list_value, i, child, if ((x = resolve(child, query, data)) != 0) return x;);
  }

  if (ast->options) {
    LOOP_NODES(ast->options, i, child, if ((x = resolve(child, query, data)) != 0) return x;);
  }

  return 0;
}
