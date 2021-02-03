#include <fjb/AST.h>
#include <fjb/plugin.h>
#include <stdio.h>
#include <string.h>

/**
 * Converts normal functions to arrow functions
 */
AST_T* function_to_arrow_function(AST_T* ast)
{
  if (ast->type != AST_FUNCTION)
    return ast;
  printf("hello\n");

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  AST_T* arrow_function = init_ast(AST_ARROW_DEFINITION);
  arrow_function->body = ast->body && ast->body->list_value->size ? ast->body : 0;

  if (!arrow_function->body) {
    arrow_function->body = init_ast(AST_NAME);
    arrow_function->body->name = strdup("null");
  }

  arrow_function->list_value = ast->list_value;
  assignment->left = init_ast(AST_NAME);

  assignment->value = arrow_function;

  if (ast->name) {
    assignment->left->name = strdup(ast->name);
  }

  return assignment;
}

/**
 * Entry point of plugin,
 * needs to be named "hook"
 */
void* hook(int type, void* ptr, fjb_env_T* env)
{
  switch (type) {
    case HOOK_AFTER_EVAL: return function_to_arrow_function((AST_T*)ptr); break;
    default: return ptr; break;
  }

  return ptr;
}
