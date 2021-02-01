#include "include/gen_jsx.h"
#include "include/gen.h"
#include "include/js.h"
#include "include/js/jsx.js.h"
#include "include/js/jsx_add_listener.js.h"
#include "include/js/jsx_append.js.h"
#include "include/js/jsx_attr.js.h"
#include "include/js/jsx_close.js.h"
#include "include/js/jsx_ptr.js.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

char* gen_jsx_template_string(AST_T* ast, fjb_env_T* env)
{
  char* m = "`";
  char* str = 0;

  str = str_append(&str, "document.createTextNode(");
  str = str_append(&str, m);
  str = str_append(&str, "${");

  if (ast->string_value) {
    str = str_append(&str, ast->string_value);
  } else if (ast->expr) {
    char* v = gen(ast->expr, env);
    str = str_append(&str, v);
  }
  str = str_append(&str, "}");
  str = str_append(&str, m);
  str = str_append(&str, ")");

  return str;
}

char* gen_jsx_text(AST_T* ast, fjb_env_T* env)
{
  char* m = "\"";
  char* str = 0;

  str = str_append(&str, "document.createTextNode(");
  str = str_append(&str, m);

  if (ast->string_value) {
    str = str_append(&str, ast->string_value);
  } else if (ast->expr) {
    char* v = gen(ast->expr, env);
    str = str_append(&str, v);
  }
  str = str_append(&str, m);
  str = str_append(&str, ")");

  return str;
}

char* gen_jsx_body(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = gen_jsx(child, env);

    if (childstr) {
      str = str_append(&str, "__jsx_append(parent,");
      str = str_append(&str, childstr);
      str = str_append(&str, ")");
    }

    if (i < ast->list_value->size) { str = str_append(&str, ";"); });

  return str ? str : strdup("");
}

char* gen_jsx_attributes(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  const char* TEMPLATE = (const char*)_tmp_jsx_attr_js;
  unsigned int TEMPLATE_LEN = _tmp_jsx_attr_js_len;

  const char* TEMPLATE_LISTENER = (const char*)_tmp_jsx_add_listener_js;
  unsigned int TEMPLATE_LISTENER_LEN = _tmp_jsx_add_listener_js_len;

  LOOP_NODES(
    ast->options, i, child, char* name = ast_get_string(child); if (!name) continue;
    if (!child->value) continue;

    const char* template = (const char*)TEMPLATE; unsigned int len = TEMPLATE_LEN;

    if (is_js_event(name)) {
      template = TEMPLATE_LISTENER;
      len = TEMPLATE_LISTENER_LEN;
    }

    char* value = gen(child->value, env);
    char* buff = calloc(strlen(name) + strlen(value) + len + 1, sizeof(char));
    sprintf(buff, template, name, value);

    str = str_append(&str, buff);

    /* if (i < ast->list_value->size - 1) { str = str_append(&str, ""); }*/);

  return str ? str : strdup("");
}

char* gen_jsx_call(AST_T* ast, fjb_env_T* env)
{
  char* value = 0;

  AST_T* call_ast = init_ast(AST_CALL);
  call_ast->name = strdup(ast->ptr->name);
  call_ast->list_value = list_copy(ast->options);

  if ((ast->ptr && ast->ptr->type == AST_FUNCTION) ||
      (ast->ptr && ast->ptr->value && ast->ptr->value->type == AST_FUNCTION)) {
    AST_T* state = init_ast(AST_STATE);
    state->string_value = strdup("new");
    state->value = call_ast;

    value = gen(state, env);
  } else {
    value = gen(call_ast, env);
  }

  return value;
}

char* gen_jsx_element(AST_T* ast, fjb_env_T* env)
{
  const char* TEMPLATE = (const char*)(ast->ptr ? _tmp_jsx_ptr_js : _tmp_jsx_js);
  unsigned int TEMPLATE_LEN = ast->ptr ? _tmp_jsx_ptr_js_len : _tmp_jsx_js_len;
  char* name = ast_get_string(ast);
  char* func_name = ast->ptr ? gen_jsx_call(ast, env) : strdup("document.createElement");
  char* attr = gen_jsx_attributes(ast, env);
  char* body = ast->body ? gen_jsx_body(ast->body, env) : strdup("");
  char* call_args = ast->options && ast->options->size ? gen_tuple(ast->options, env) : strdup("");
  char* value = calloc(TEMPLATE_LEN + strlen(name) + strlen(call_args) + strlen(func_name) +
                         strlen(attr) + strlen(body) + 1,
                       sizeof(char));

  sprintf(value, TEMPLATE, func_name, name, attr, body, call_args);

  return value;
}

char* gen_jsx_template_value(AST_T* ast, fjb_env_T* env)
{
  char* value = 0;
  if (ast->expr) {
    char* exprstr = gen(ast->expr, env);
    value = str_append(&value, exprstr);
    return value;
  }

  return strdup("");
}

char* gen_jsx(AST_T* ast, fjb_env_T* env)
{
  switch (ast->type) {
    case AST_JSX_ELEMENT: return gen_jsx_element(ast, env); break;
    case AST_JSX_COMPOUND: return gen_jsx_body(ast, env); break;
    case AST_JSX_TEMPLATE_VALUE: return gen_jsx_template_value(ast, env); break;
    case AST_JSX_TEMPLATE_STRING: return gen_jsx_template_string(ast, env); break;
    case AST_JSX_TEXT: return gen_jsx_text(ast, env); break;
    default: {
      printf("[Gen(JSX)]: Missing generator for `%d`\n", ast->type);
      exit(1);
    } break;
  }

  return 0;
}
