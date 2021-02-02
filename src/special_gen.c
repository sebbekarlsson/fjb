#include "include/special_gen.h"
#include "include/env.h"
#include "include/gen.h"
#include "include/imported.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

compiler_result_T* special_gen_json(fjb_env_T* env, list_T* imports)
{
  if (imports && !imports->size)
    return init_compiler_result(env->source, env->filepath);

  char* value = 0;
  char* str = strdup(env->source);

  AST_T* imp = (AST_T*)imports->items[0];
  char* imp_name = ast_get_string(imp);

  if (!imp)
    return init_compiler_result(env->source, env->filepath);

  AST_T* json_ast = init_ast(AST_RAW);
  json_ast->string_value = str;

  if (!env->aliased_import) {
    AST_T* assignment = init_ast(AST_ASSIGNMENT);
    assignment->flags = NEW_STACK;
    assignment->left = init_ast(AST_NAME);
    assignment->left->name = strdup(imp_name);
    assignment->name = strdup(imp_name);
    assignment->value = json_ast;

    AST_T* flag = init_ast(AST_NAME);
    flag->name = strdup("let");
    list_push(assignment->flags, flag);

    assignment->exported = 1;

    map_set(FJB_ENV->imports, assignment->name, init_imported(assignment->name, 0, assignment));
    map_set(FJB_ENV->assignments, assignment->name, assignment);
    list_push(FJB_ENV->search_index, assignment);

    value = gen(assignment, env);
  } else if (env->aliased_import) {
    AST_T* state = init_ast(AST_STATE);
    state->value = json_ast;
    state->string_value = strdup("return");

    value = gen(state, env);
  }

  value = str_append(&value, ";");

  compiler_result_T* result = init_compiler_result(value, strdup(env->filepath));
  result->node = json_ast;

  return result;
}

compiler_result_T* special_gen_css(fjb_env_T* env, list_T* imports)
{
  if (imports && !imports->size)
    return init_compiler_result(env->source, env->filepath);

  char* value = 0;
  char* str = strdup(env->source);

  AST_T* imp = (AST_T*)imports->items[0];
  char* imp_name = ast_get_string(imp);

  if (!imp)
    return init_compiler_result(env->source, env->filepath);

  AST_T* css_ast = init_ast(AST_TEMPLATE_STRING);
  css_ast->string_value = str;

  if (!env->aliased_import) {
    AST_T* assignment = init_ast(AST_ASSIGNMENT);
    assignment->flags = NEW_STACK;
    assignment->left = init_ast(AST_NAME);
    assignment->left->name = strdup(imp_name);
    assignment->name = strdup(imp_name);
    assignment->value = css_ast;

    AST_T* flag = init_ast(AST_NAME);
    flag->name = strdup("let");
    list_push(assignment->flags, flag);

    assignment->exported = 1;

    value = gen(assignment, env);
  } else if (env->aliased_import) {
    AST_T* state = init_ast(AST_STATE);
    state->value = css_ast;
    state->string_value = strdup("return");

    value = gen(state, env);
  }

  value = str_append(&value, ";");

  compiler_result_T* result = init_compiler_result(value, strdup(env->filepath));
  result->node = css_ast;

  return result;
}

compiler_result_T* special_gen(fjb_env_T* env, char* ext, list_T* imports)
{
  if (strcasecmp(ext, ".json") == 0) {
    return special_gen_json(env, imports);
  }
  if (strcasecmp(ext, ".css") == 0) {
    return special_gen_css(env, imports);
  }

  return 0;
}
