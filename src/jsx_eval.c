#include "include/jsx_eval.h"
#include <stdio.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

AST_T* eval_jsx_template_value(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->expr) {
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  }

  return ast;
}

AST_T* eval_jsx_element(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  LOOP_NODES(ast->options, i, child, visitor_visit(visitor, child, stack););

  query_T query;
  query.name = ast_get_string(ast);
  query.type = AST_FUNCTION;

  ast->body = eval_jsx(visitor, ast->body, stack);

  AST_T* def = 0;

  list_T* search_index = list_merge(FJB_ENV->search_index, stack);

  if (ast->name) {
    LOOP_NODES(
      search_index, i, child, char* name = ast_get_string(child);

      if (!name) continue;

      if (child->type == AST_ASSIGNMENT || child->type == AST_FUNCTION) {
        if (strcmp(name, ast->name) == 0) {
          def = child;
          break;
        }
      });
  }

  if (def) {
    ast->ptr = def;
  }

  /*if (ast->ptr)
  {
  AST_T* call_ast = init_ast(AST_CALL);
  //call_ast->ptr = def;
  call_ast->name = strdup(ast->ptr->name);
  call_ast->list_value = list_copy(ast->options);

  if ((ast->ptr && ast->ptr->type == AST_FUNCTION) ||
      (ast->ptr && ast->ptr->value && ast->ptr->value->type == AST_FUNCTION)) {
    AST_T* state = init_ast(AST_STATE);
    state->string_value = strdup("new");
    state->value = call_ast;

    return state;
  } else {
    return call_ast;
  }
  }
    ast->ptr = 0;*/

  return ast;
}

AST_T* eval_jsx_compound(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  LOOP_NODES(ast->list_value, i, child,
             ast->list_value->items[i] = eval_jsx(visitor, child, stack););

  return ast;
}

AST_T* eval_jsx(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (!ast)
    return 0;

  switch (ast->type) {
    case AST_JSX_ELEMENT: return eval_jsx_element(visitor, ast, stack); break;
    case AST_JSX_COMPOUND: return eval_jsx_compound(visitor, ast, stack); break;
    case AST_JSX_TEMPLATE_VALUE: return eval_jsx_template_value(visitor, ast, stack); break;
    case AST_JSX_TEXT: return ast; break;
    case AST_JSX_TEMPLATE_STRING: return ast; break;
    default: {
      printf("eval_jsx received unsupported AST.\n");
      exit(1);
    }
  }

  return ast;
}
