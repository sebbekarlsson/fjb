#include "include/gen_jsx.h"
#include "include/gen.h"
#include "include/js/jsx.js.h"
#include "include/js/jsx_append.js.h"
#include "include/js/jsx_attr.js.h"
#include "include/js/jsx_close.js.h"
#include "include/js/jsx_ptr.js.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <string.h>

char* gen_jsx_template_string(AST_T* ast, compiler_flags_T* flags)
{
  char* m = "`";
  char* str = 0;

  str = str_append(&str, "document.createTextNode(");
  str = str_append(&str, m);
  str = str_append(&str, ast->string_value);
  str = str_append(&str, m);
  str = str_append(&str, ")");

  return str;
}

char* gen_jsx_body(AST_T* ast, compiler_flags_T* flags)
{
  char* str = 0;

  LOOP_NODES(
    ast->list_value, i, child, char* childstr = gen_jsx(child, flags);

    if (childstr) {
      str = str_append(&str, "parent.appendChild(");
      str = str_append(&str, childstr);
      str = str_append(&str, ")");
    }

    if (i < ast->list_value->size) { str = str_append(&str, "\n"); });

  return str ? str : strdup("");
}

char* gen_jsx_attributes(AST_T* ast, compiler_flags_T* flags)
{
  char* str = 0;

  const char* TEMPLATE = (const char*)_tmp_jsx_attr_js;
  unsigned int TEMPLATE_LEN = _tmp_jsx_attr_js_len;
  LOOP_NODES(
    ast->options, i, child, char* name = ast_get_string(child); if (!name) continue;
    if (!child->value) continue;

    char* value = gen(child->value, flags);
    char* buff = calloc(strlen(name) + strlen(value) + TEMPLATE_LEN + 1, sizeof(char));
    sprintf(buff, TEMPLATE, name, value);

    str = str_append(&str, buff);

    if (i < ast->list_value->size - 1) { str = str_append(&str, "\n"); });

  return str ? str : strdup("");
}

char* gen_jsx_call(AST_T* ast, compiler_flags_T* flags)
{
  AST_T* call_ast = init_ast(AST_CALL);
  call_ast->name = strdup(ast->ptr->name);
  call_ast->list_value = list_copy(ast->options);

  char* value = gen(call_ast, flags);

  return value;
}

char* gen_jsx_element(AST_T* ast, compiler_flags_T* flags)
{
  const char* TEMPLATE = (const char*)(ast->ptr ? _tmp_jsx_ptr_js : _tmp_jsx_js);
  unsigned int TEMPLATE_LEN = ast->ptr ? _tmp_jsx_ptr_js_len : _tmp_jsx_js_len;
  char* name = ast_get_string(ast);
  char* func_name = ast->ptr ? gen_jsx_call(ast, flags) : strdup("document.createElement");
  char* attr = gen_jsx_attributes(ast, flags);
  char* body = ast->body ? gen_jsx_body(ast->body, flags) : strdup("");
  char* value =
    calloc(TEMPLATE_LEN + strlen(name) + strlen(func_name) + strlen(attr) + strlen(body) + 1,
           sizeof(char));
  sprintf(value, TEMPLATE, func_name, name, attr, body);

  return value;
}

char* gen_jsx(AST_T* ast, compiler_flags_T* flags)
{
  switch (ast->type) {
    case AST_JSX_ELEMENT: return gen_jsx_element(ast, flags); break;
    case AST_JSX_COMPOUND: return gen_jsx_body(ast, flags); break;
    case AST_JSX_TEMPLATE_STRING:
    case AST_JSX_TEXT: return gen_jsx_template_string(ast, flags); break;
    default: {
      printf("[Gen(JSX)]: Missing generator for `%d`\n", ast->type);
      exit(1);
    } break;
  }

  return 0;
}
