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
  if (FJB_ENV->map && FJB_ENV->map->len_used_buckets && FJB_ENV->map->nrkeys) {
    ast->stack_frame = map_copy(FJB_ENV->map);
  }

  if (ast->name_ast)
    ast->name_ast = visitor_visit(visitor, ast->name_ast, stack);

  LOOP_NODES(ast->options, i, child, visitor_visit(visitor, child, stack););

  ast->body = eval_jsx(visitor, ast->body, stack);

  AST_T* def = (AST_T*)map_get_value(ast->stack_frame, ast->name);

  if (def) {
    if (def->type == AST_ASSIGNMENT && def->value && def->name && ast->name_ast &&
        ast->name_ast->name) {
      char* defname = strdup(def->name);

      def = def->value;

      if (def->type == AST_OBJECT) {
        AST_T* binop = init_ast(AST_BINOP);
        binop->left = init_ast(AST_NAME);
        binop->left->name = defname;
        binop->token = init_token(".", TOKEN_DOT);
        binop->right = init_ast(AST_NAME);
        binop->right->name = strdup(ast->name_ast->name);
        def = binop;
      }
    } else if (def->name) {
      AST_T* ast_name = init_ast(AST_NAME);
      ast_name->name = strdup(def->name);
      def = ast_name;
    }
    ast->ptr = def;
  }

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

  FJB_ENV->is_using_jsx = 1;

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
