#include "include/eval_jsx.h"
#include "include/jsx.h"
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
  if (!ast->stack_frame && FJB_ENV->map && FJB_ENV->map->len_used_buckets && FJB_ENV->map->nrkeys) {
    ast->stack_frame = map_copy(FJB_ENV->map);
  }

  if (ast->name_ast)
    ast->name_ast = visitor_visit(visitor, ast->name_ast, stack);

  int jsx_type = fjb_get_jsx_type();

  LOOP_NODES(ast->options, i, child,
             jsx_type == JSX_DEFAULT ? eval_jsx(visitor, child, stack)
                                     : visitor_visit(visitor, child, stack));

  if (ast->body) {
    ast->body = jsx_type == JSX_DEFAULT ? eval_jsx(visitor, ast->body, stack)
                                        : visitor_visit(visitor, ast->body, stack);
  }
  AST_T* def = 0;

  if (!def)
    def = (AST_T*)map_get_value(FJB_ENV->functions, ast->name);

  if (!def)
    def = (AST_T*)map_get_value(FJB_ENV->assignments, ast->name);

  if (!def && ast->stack_frame)
    def = (AST_T*)map_get_value(ast->stack_frame, ast->name);

  if (!def)
    def = (AST_T*)map_get_value(FJB_ENV->map, ast->name);

  if (def && (!(def->type == AST_FUNCTION || def->type == AST_ARROW_DEFINITION ||
                def->type == AST_ASSIGNMENT)))
    def = 0;

  if (def && def->type == AST_ASSIGNMENT && def->value &&
      !(def->value->type == AST_FUNCTION || def->value->type == AST_ARROW_DEFINITION ||
        def->value->type == AST_NAME))
    def = 0;

  if (def && def->name) {
    if (def->type == AST_FUNCTION && def->list_value && def->list_value->size && ast->options) {
      LOOP_NODES(
        def->list_value, i, child, char* k = child->name; if (!k) continue;

        unsigned int len = ast->options->size; for (unsigned int j = 0; j < len; j++) {
          AST_T* opt = (AST_T*)ast->options->items[j];
          if (!opt->name)
            continue;
          if (strcmp(opt->name, child->name) == 0) {
            list_remove(ast->options, opt, 0);
            list_push(ast->options, opt);
          }
        });
    }

    AST_T* ast_name = init_ast(AST_NAME);
    ast_name->name = strdup(def->name);
    def = ast_name;

    ast->ptr = def;
  }

  return ast;

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

AST_T* eval_jsx_assignment(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  AST_T* def = (AST_T*)map_get_value(FJB_ENV->functions, ast->name);

  if (ast->value) {
    ast->value = visitor_visit(visitor, ast->value, stack);

    if (ast->value->expr)
      ast->value->expr = visitor_visit(visitor, ast->value->expr, stack);

    AST_T* ptr = ast->value->ptr                             ? ast->value->ptr
                 : ast->value->expr && ast->value->expr->ptr ? ast->value->expr->ptr
                                                             : 0;

    if (ptr) {
      if (ptr->name) {
        // ast->value = init_ast_name(ptr->name);
      }
    }
  }

  return ast;
}

AST_T* eval_jsx_binop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return visitor_visit(visitor, ast, stack);
}

AST_T* eval_jsx_name(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return visitor_visit(visitor, ast, stack);
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
    case AST_BINOP: return eval_jsx_binop(visitor, ast, stack); break;
    case AST_NAME: return eval_jsx_name(visitor, ast, stack); break;
    case AST_ASSIGNMENT: return eval_jsx_assignment(visitor, ast, stack); break;
    default: {
      printf("eval_jsx received unsupported AST.\n");
      exit(1);
    }
  }

  return ast;
}
