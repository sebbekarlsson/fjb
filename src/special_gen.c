#include "include/special_gen.h"
#include "include/gen.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

compiler_result_T* special_gen_json(compiler_flags_T* flags)
{
  if (!(flags->imports && flags->imports->size))
    return init_compiler_result(flags->source, flags->filepath);

  char* value = 0;
  char* str = strdup(flags->source);

  AST_T* imp = (AST_T*)flags->imports->items[0];
  char* imp_name = ast_get_string(imp);

  AST_T* json_ast = init_ast(AST_RAW);
  json_ast->string_value = str;

  if (!flags->aliased_import) {
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

    value = gen(assignment, flags);
  } else if (flags->aliased_import) {
    AST_T* state = init_ast(AST_STATE);
    state->value = json_ast;
    state->string_value = strdup("return");

    value = gen(state, flags);
  }

  value = str_append(&value, ";");

  compiler_result_T* result = init_compiler_result(value, strdup(flags->filepath));

  return result;
}

compiler_result_T* special_gen_css(compiler_flags_T* flags)
{
  if (!(flags->imports && flags->imports->size))
    return init_compiler_result(flags->source, flags->filepath);

  char* value = 0;
  char* str = strdup(flags->source);

  AST_T* imp = (AST_T*)flags->imports->items[0];
  char* imp_name = ast_get_string(imp);

  AST_T* css_ast = init_ast(AST_TEMPLATE_STRING);
  css_ast->string_value = str;

  if (!flags->aliased_import) {
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

    value = gen(assignment, flags);
  } else if (flags->aliased_import) {
    AST_T* state = init_ast(AST_STATE);
    state->value = css_ast;
    state->string_value = strdup("return");

    value = gen(state, flags);
  }

  value = str_append(&value, ";");

  compiler_result_T* result = init_compiler_result(value, strdup(flags->filepath));

  return result;
}

compiler_result_T* special_gen(compiler_flags_T* flags, char* ext)
{
  if (strcasecmp(ext, ".json") == 0) {
    return special_gen_json(flags);
  }
  if (strcasecmp(ext, ".css") == 0) {
    return special_gen_css(flags);
  }

  printf("[Error]: Cannot load files with extension `%s`", ext);
  exit(1);
}
