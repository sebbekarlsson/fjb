#include "include/emit_jsx.h"
#include "include/emit.h"
#include "include/env.h"
#include "include/imported.h"
#include "include/js.h"
#include "include/js/jsx.js.h"
#include "include/js/jsx_add_listener.js.h"
#include "include/js/jsx_append.js.h"
#include "include/js/jsx_attr.js.h"
#include "include/js/jsx_ptr.js.h"
#include "include/js/jsx_react.js.h"
#include "include/js/jsx_react_ptr.js.h"
#include "include/jsx.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

const char* get_create_element_string()
{
  int jsx_type = fjb_get_jsx_type();

  switch (jsx_type) {
    case JSX_DEFAULT: return "document.createElement"; break;
    case JSX_REACT: return "React.createElement"; break;
    default: return "noop("; break;
  }
}

const char* get_create_text_node_string()
{
  int jsx_type = fjb_get_jsx_type();

  switch (jsx_type) {
    case JSX_DEFAULT: return "document.createTextNode("; break;
    case JSX_REACT: return "React_createTextNode("; break;
    default: return "noop("; break;
  }
}

char* emit_jsx_template_string(AST_T* ast)
{
  char* m = "`";
  char* str = 0;

  str = str_append(&str, get_create_text_node_string());
  str = str_append(&str, m);
  str = str_append(&str, "${");

  if (ast->alias) {
    str = str_append(&str, ast->alias);
  } else if (ast->string_value) {
    str = str_append(&str, ast->string_value);
  } else if (ast->expr) {
    char* v = emit(ast->expr);
    str = str_append(&str, v);
  }
  str = str_append(&str, "}");
  str = str_append(&str, m);
  str = str_append(&str, ")");

  return str;
}

char* emit_jsx_text(AST_T* ast)
{
  char* m = "`";
  char* str = 0;

  str = str_append(&str, get_create_text_node_string());
  str = str_append(&str, m);

  if (ast->string_value) {
    str = str_append(&str, ast->string_value);
  } else if (ast->expr) {
    char* v = emit(ast->expr);
    str = str_append(&str, v);
  }
  str = str_append(&str, m);
  str = str_append(&str, ")");

  return str;
}

char* emit_jsx_react_body(AST_T* ast)
{
  char* str = 0;

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = emit_jsx(child);

    if (childstr) {
      str = str_append(&str, childstr);
      free(childstr);
    }

    if (i < ast->list_value->size - 1) { str = str_append(&str, ","); });

  return str ? str : strdup("");
}

char* emit_jsx_body(AST_T* ast)
{
  char* str = 0;

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = emit_jsx(child);

    if (childstr) {
      str = str_append(&str, "__jsx_append(parent,");
      str = str_append(&str, childstr);
      str = str_append(&str, ")");
      free(childstr);
    }

    if (i < ast->list_value->size) { str = str_append(&str, ";"); });

  return str ? str : strdup("");
}

char* emit_jsx_attributes(AST_T* ast)
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

    char* value = emit(child->value);
    char* buff = calloc(strlen(name) + strlen(value) + len + 1, sizeof(char));
    sprintf(buff, template, name, value);

    str = str_append(&str, buff);

    /* if (i < ast->list_value->size - 1) { str = str_append(&str, ""); }*/);

  return str ? str : strdup("");
}

char* emit_jsx_call(AST_T* ast)
{
  char* value = 0;

  AST_T* call_ast = init_ast(AST_CALL);
  call_ast->left = ast->ptr;
  call_ast->list_value = list_copy(ast->options);

  AST_T* state = init_ast(AST_STATE);
  state->string_value = strdup("new");
  state->value = call_ast;
  gc_mark(FJB_ENV->GC, state);

  value = emit(state);

  return value;
}

char* emit_jsx_element(AST_T* ast)
{
  const char* TEMPLATE = (const char*)(ast->ptr ? _tmp_jsx_ptr_js : _tmp_jsx_js);
  unsigned int TEMPLATE_LEN = ast->ptr ? _tmp_jsx_js_len : _tmp_jsx_js_len;
  char* name = ast_get_string(ast);
  char* func_name = ast->ptr ? emit_jsx_call(ast) : strdup(get_create_element_string());
  char* attr = emit_jsx_attributes(ast);
  char* body = ast->body ? emit_jsx_body(ast->body) : strdup("");
  char* call_args = ast->options && ast->options->size ? emit_tuple(ast->options) : strdup("");
  char* value = calloc(TEMPLATE_LEN + strlen(name) + strlen(call_args) + strlen(func_name) +
                         strlen(attr) + strlen(body) + 1,
                       sizeof(char));

  sprintf(value, TEMPLATE, func_name, name, attr, body, call_args);

  return value;
}

char* emit_jsx_react_element(AST_T* ast)
{
  const char* TEMPLATE = (const char*)(ast->ptr ? _tmp_jsx_react_ptr_js : _tmp_jsx_react_js);
  unsigned int TEMPLATE_LEN = ast->ptr ? _tmp_jsx_react_ptr_js_len : _tmp_jsx_react_js_len;
  char* name = ast_get_string(ast);
  char* func_name = name; // ast->ptr ? emit_jsx_call(ast) : name;
  char* body = ast->body ? emit_jsx_react_body(ast->body) : strdup("");
  char* attr = ast->options && ast->options->size ? emit_tuple(ast->options) : strdup("");
  char* value =
    calloc(TEMPLATE_LEN + strlen(name) + strlen(func_name) + strlen(attr) + strlen(body) + 1,
           sizeof(char));

  sprintf(value, TEMPLATE, func_name, attr, body);

  return value;
}

char* emit_jsx_template_value(AST_T* ast)
{
  char* value = 0;
  if (ast->expr) {
    char* exprstr = emit(ast->expr);
    value = str_append(&value, exprstr);
    return value;
  }

  return strdup("");
}

char* emit_jsx(AST_T* ast)
{
  unsigned int jsx_type = fjb_get_jsx_type();

  if (jsx_type == JSX_DEFAULT) {
    switch (ast->type) {
      case AST_JSX_ELEMENT: return emit_jsx_element(ast); break;
      case AST_JSX_COMPOUND: return emit_jsx_body(ast); break;
      case AST_JSX_TEMPLATE_VALUE: return emit_jsx_template_value(ast); break;
      case AST_JSX_TEMPLATE_STRING: return emit_jsx_template_string(ast); break;
      case AST_JSX_TEXT: return emit_jsx_text(ast); break;
      case AST_CALL:
      case AST_STATE: return emit(ast); break;
      default: {
        printf("[Gen(JSX)]: Missing emiterator for `%d`\n", ast->type);
        exit(1);
      } break;
    }
  } else if (jsx_type == JSX_REACT) {
    switch (ast->type) {
      case AST_JSX_ELEMENT: return emit_jsx_react_element(ast); break;
      case AST_JSX_COMPOUND: return emit_jsx_react_body(ast); break;
      case AST_JSX_TEMPLATE_VALUE: return emit_jsx_template_value(ast); break;
      case AST_JSX_TEMPLATE_STRING: return emit_jsx_template_string(ast); break;
      case AST_JSX_TEXT: return emit_jsx_text(ast); break;
      case AST_CALL:
      case AST_STATE: return emit(ast); break;
      default: {
        printf("[Gen(JSX)]: Missing emiterator for `%d`\n", ast->type);
        exit(1);
      } break;
    }
  }

  return 0;
}
