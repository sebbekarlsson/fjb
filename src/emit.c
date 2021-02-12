#include "include/emit.h"
#include "include/emit_jsx.h"
#include "include/io.h"
#include "include/js/arrow_definition.js.h"
#include "include/js/arrow_definition_no_brace.js.h"
#include "include/js/assignment.js.h"
#include "include/js/braced.js.h"
#include "include/js/class_definition.js.h"
#include "include/js/do_statement.js.h"
#include "include/js/do_statement_no_brace.js.h"
#include "include/js/export_def.js.h"
#include "include/js/expose_def.js.h"
#include "include/js/for_loop.js.h"
#include "include/js/function.js.h"
#include "include/js/function_class.js.h"
#include "include/js/import.js.h"
#include "include/js/module_requirement.js.h"
#include "include/js/optional_chain.js.h"
#include "include/js/require.js.h"
#include "include/js/string.js.h"
#include "include/js/switch_case.js.h"
#include "include/js/switch_statement.js.h"
#include "include/js/template_string.js.h"
#include "include/js/while_loop.js.h"
#include "include/js/while_loop_no_brace.js.h"
#include "include/package.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMPLATE(template_name, tmpname, len, ...)                                                 \
  tmpname = calloc(_tmp_##template_name##_js_len + len + 128, sizeof(char));                       \
  sprintf(tmpname, (const char*)_tmp_##template_name##_js, __VA_ARGS__);

static char* emit_args(list_T* list_value, fjb_env_T* env)
{
  char* str = 0;
  str = str_append(&str, "(");

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1 && child_str)
      str = str_append(&str, ",");

    free(child_str);
  }

  str = str_append(&str, ")");

  return str;
}

static char* emit_semi_args(list_T* list_value, fjb_env_T* env)
{
  char* str = 0;
  str = str_append(&str, "(");

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1 && child_str && child->type != AST_COLON_ASSIGNMENT &&
        child->type != AST_JSX_ELEMENT)
      str = str_append(&str, ";");

    free(child_str);
  }

  str = str_append(&str, ")");

  return str;
}

static char* emit_list(list_T* list_value, fjb_env_T* env)
{
  char* str = 0;
  str = str_append(&str, "[");

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1 && child_str)
      str = str_append(&str, ",");

    free(child_str);
  }

  str = str_append(&str, "]");

  return str;
}

char* emit_spaced_list(list_T* list_value, fjb_env_T* env)
{
  if (!list_value)
    return 0;

  char* str = 0;
  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1)
      str = str_append(&str, " ");

    free(child_str);
  }

  return str;
}

char* emit_tuple(list_T* list_value, fjb_env_T* env)
{
  char* str = 0;

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1 && child->type != AST_NOOP && child->type != AST_UNDEFINED)
      str = str_append(&str, ",");

    free(child_str);
  }

  return str;
}

/*static char* emit_semi_tuple(list_T* list_value, fjb_env_T* env)
{
  char* str = 0;

  list_T* living = list_value;

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child = (AST_T*)living->items[i];

    char* child_str = emit(child, env);

    if (!child_str)
      continue;

    str = str_append(&str, child_str);

    if (i < living->size - 1 && child->type != AST_COLON_ASSIGNMENT && child->type != AST_NOOP &&
        child->type != AST_JSX_ELEMENT)
      str = str_append(&str, ";");

    free(child_str);
  }

  return str ? str : strdup("");
}*/

char* emit(AST_T* ast, fjb_env_T* env)
{
  if (!ast) {
    printf("[Gen]: Received nil AST.\n");
    exit(1);
  }

  ast = fjb_call_all_hooks(HOOK_BEFORE_GENERATE, ast, env);

  char* body = 0;
  char* str = 0;

  if (ast->comment) {
    char buff[256];
    sprintf(buff, "\n/*%s*/\n", ast->comment);
    str = str_append(&str, buff);
  }

  switch (ast->type) {
    case AST_ARRAY: body = emit_array(ast, env); break;
    case AST_TUPLE: body = emit_tuple_ast(ast, env); break;
    case AST_HEX: body = emit_int(ast, env); break;
    case AST_BOOL: body = emit_bool(ast, env); break;
    case AST_INT: body = emit_int(ast, env); break;
    case AST_INT_MIN: body = emit_int_min(ast, env); break;
    case AST_FLOAT: body = emit_float(ast, env); break;
    case AST_STRING: body = emit_string(ast, env); break;
    case AST_RAW: body = emit_raw(ast, env); break;
    case AST_TEMPLATE_STRING: body = emit_template_string(ast, env); break;
    case AST_ARROW_DEFINITION: body = emit_arrow_definition(ast, env); break;
    case AST_ASSIGNMENT: body = emit_assignment(ast, env); break;
    case AST_CASE: body = emit_case(ast, env); break;
    case AST_COLON_ASSIGNMENT: body = emit_colon_assignment(ast, env); break;
    case AST_WHILE: body = emit_while(ast, env); break;
    case AST_FOR: body = emit_for(ast, env); break;
    case AST_COMPOUND: body = emit_compound(ast, env); break;
    case AST_IMPORT: body = emit_import(ast, env); break;
    case AST_UNDEFINED: body = emit_undefined(ast, env); break;
    case AST_CALL: body = emit_call(ast, env); break;
    case AST_CLASS_FUNCTION:
    case AST_FUNCTION: body = emit_function(ast, env); break;
    case AST_SCOPE: body = emit_scope(ast, env); break;
    case AST_SIGNATURE: body = emit_signature(ast, env); break;
    case AST_NAME: body = emit_name(ast, env); break;
    case AST_STATE: body = emit_state(ast, env); break;
    case AST_BINOP: body = emit_binop(ast, env); break;
    case AST_UNOP: body = emit_unop(ast, env); break;
    case AST_INCREMENT: body = emit_increment(ast, env); break;
    case AST_DECREMENT: body = emit_decrement(ast, env); break;
    case AST_REGEX: body = emit_regex(ast, env); break;
    case AST_OBJECT: body = emit_object(ast, env); break;
    case AST_CONDITION: body = emit_condition(ast, env); break;
    case AST_SWITCH: body = emit_switch(ast, env); break;
    case AST_TRY: body = emit_try(ast, env); break;
    case AST_CLASS: body = emit_class(ast, env); break;
    case AST_LABEL: body = emit_label(ast, env); break;
    case AST_TERNARY: body = emit_ternary(ast, env); break;
    case AST_DO: body = emit_do(ast, env); break;
    case AST_JSX_TEXT:
    case AST_JSX_TEMPLATE_STRING:
    case AST_JSX_TEMPLATE_VALUE:
    case AST_JSX_ELEMENT: body = emit_jsx(ast, env); break;
    case AST_NOOP: body = emit_noop(ast, env); break;
    default: {
      printf("[Gen]: missing emiterator for `%d`", ast->type);
      exit(1);
    } break;
  }

  if (ast->flags) {
    for (unsigned int i = 0; i < ast->flags->size; i++) {
      AST_T* ast_flag = (AST_T*)ast->flags->items[i];
      char* ast_flag_str = emit(ast_flag, env);
      str = str_append(&str, ast_flag_str);
      free(ast_flag_str);
      str = str_append(&str, " ");
    }
  }

  char* leftstr = 0;
  char* rightstr = 0;

  if (ast->left && ast->type != AST_CLASS_FUNCTION && ast->type != AST_BINOP) {
    leftstr = emit(ast->left, env);
  }

  if (ast->right && ast->type != AST_DO && ast->type != AST_BINOP) {
    rightstr = emit(ast->right, env);
  }

  if (ast->capsulated && body) {
    str = str_append(&str, "(");

    if (leftstr) {
      str = str_append(&str, leftstr);
      free(leftstr);
    }

    str = str_append(&str, body);

    if (rightstr) {
      str = str_append(&str, rightstr);
      free(rightstr);
    }

    str = str_append(&str, ")");

    free(body);
  } else if (body) {
    if (leftstr) {
      str = str_append(&str, leftstr);
      free(leftstr);
    }

    str = str_append(&str, body);

    if (rightstr) {
      str = str_append(&str, rightstr);
      free(rightstr);
    }
  }

  AST_T* next = ast->next;

  if (next) {
    if (next)
      str = str_append(&str, ",");

    char* nextstr = emit(next, env);

    if (nextstr) {
      str = str_append(&str, nextstr);
      free(nextstr);
    }
  }

  str = fjb_call_all_hooks(HOOK_AFTER_GENERATE, str, env);

  return str;
}
char* emit_array(AST_T* ast, fjb_env_T* env)
{
  return emit_list(ast->list_value, env);
}

char* emit_tuple_ast(AST_T* ast, fjb_env_T* env)
{
  return emit_tuple(ast->list_value, env);
}
char* emit_int(AST_T* ast, fjb_env_T* env)
{
  return int_to_str(ast->int_value);
}

char* emit_bool(AST_T* ast, fjb_env_T* env)
{
  return int_to_str(ast->bool_value);
}

char* emit_int_min(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->string_value);
}

char* emit_hex(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->string_value);
}

char* emit_raw(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->string_value);
}

char* emit_float(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->string_value ? ast->string_value : float_to_str(ast->float_value));
}

char* emit_string(AST_T* ast, fjb_env_T* env)
{
  char* m = (ast->token && ast->token->c) ? charstr(ast->token->c) : strdup("\"");
  char* str;

  TEMPLATE(string, str, (strlen(m) * 2) + strlen(ast->string_value), m, ast->string_value, m);

  if (m)
    free(m);

  return str;
}

char* emit_template_string(AST_T* ast, fjb_env_T* env)
{
  char* buff;
  TEMPLATE(template_string, buff, strlen(ast->string_value), ast->string_value);

  return buff;
}

char* emit_arrow_definition(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  char* args_str = emit_args(ast->list_value, env);
  char* bodystr = emit(ast->body, env);

  if (ast->body && ast->body->type == AST_COMPOUND) {
    TEMPLATE(arrow_definition, str, strlen(args_str) + strlen(bodystr), args_str, bodystr);
  } else if (ast->body) {
    TEMPLATE(arrow_definition_no_brace, str, strlen(args_str) + strlen(bodystr), args_str, bodystr);
  }

  if (args_str)
    free(args_str);
  if (bodystr)
    free(bodystr);

  return str;
}

char* emit_assignment(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* valuestr = 0;
  char* expose_str = 0;

  if (ast->value) {
    if (ast->value->dead)
      return strdup("");
    valuestr = emit(ast->value, env);
  } else {
    return strdup("");
  }

  if (ast->not_exported == 0) {
    if ((ast->parent && ast->parent->type == AST_FUNCTION && env->imports && ast->name &&
         ast->flags) ||
        ast->exported) {
      TEMPLATE(expose_def, expose_str, strlen(ast->name) * 2, ast->name, ast->name);
    }
  }

  if (!valuestr)
    valuestr = strdup("");

  TEMPLATE(assignment,
           str,
           strlen(valuestr) + (expose_str ? strlen(expose_str) : 0),
           valuestr,
           expose_str ? expose_str : "");

  if (valuestr)
    free(valuestr);

  return str;
}

char* emit_colon_assignment(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  if (ast->left || ast->name) {
    if (ast->name && !ast->left) {
      str = str_append(&str, ast->name);
    }
    str = str_append(&str, " ");

    if (ast->expr) {
      char* exprstr = emit(ast->expr, env);
      if (exprstr) {
        str = str_append(&str, exprstr);
        free(exprstr);
      }
    }

    if (!(ast->expr && ast->expr->type == AST_COLON_ASSIGNMENT))
      str = str_append(&str, ":");
  }

  return str;
}

char* emit_case(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* condition_str = ast->condition ? emit(ast->condition, env) : strdup("");
  char* valuestr = ast->value ? emit(ast->value, env) : strdup("");
  char* name = strdup(ast->name);

  TEMPLATE(switch_case,
           str,
           strlen(name) + strlen(condition_str) + strlen(valuestr),
           name,
           condition_str,
           valuestr);

  return str;
}

char* emit_while(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* body_str;
  char* expr_str;

  expr_str = ast->expr ? emit(ast->expr, env) : strdup("");
  body_str = ast->body ? emit(ast->body, env) : strdup("\n");

  if (ast->body) {
    TEMPLATE(while_loop, str, strlen(expr_str) + strlen(body_str), expr_str, body_str);
  } else {
    TEMPLATE(while_loop_no_brace, str, strlen(expr_str), expr_str);
  }

  if (expr_str)
    free(expr_str);
  if (body_str)
    free(body_str);

  return str;
}

char* emit_for(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* args_str = emit_semi_args(ast->list_value, env);
  char* body_str = ast->body ? emit(ast->body, env) : strdup("\n");

  TEMPLATE(for_loop, str, strlen(body_str) + strlen(args_str), args_str, body_str);

  if (body_str)
    free(body_str);
  if (args_str)
    free(args_str);

  return str;
}

char* emit_compound(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  list_T* living = ast->list_value;

  if (!living || (living && !living->size))
    return strdup("");

  for (unsigned int i = 0; i < living->size; i++) {
    AST_T* child_ast = (AST_T*)living->items[i];

    if (child_ast->string_value && strcmp(child_ast->string_value, "use strict") == 0)
      continue;

    char* child_str = emit(child_ast, env);
    if (!child_str)
      continue;

    if (child_ast->type != AST_IMPORT && child_ast->type != AST_UNDEFINED &&
        child_ast->type != AST_LABEL && child_ast->type != AST_NOOP &&
        child_ast->type != AST_COLON_ASSIGNMENT && child_ast->type != AST_JSX_ELEMENT &&
        child_ast->type != AST_DO && child_str && i < living->size - 1) {
      child_str = str_append(&child_str, ";");
    }

    str = str_append(&str, child_str);

    free(child_str);
  }

  return str;
}

char* emit_import(AST_T* ast, fjb_env_T* env)
{
  if (ast->dead)
    return strdup("");

  char* encoding = ast->alias ? strdup(ast->alias) : ast->encoding ? strdup(ast->encoding) : 0;

  char* str = 0;
  char* k = ast->node             ? emit(ast->node, env)
            : ast->compiled_value ? strdup(ast->compiled_value)
                                  : 0;

  if (!k) {
    return strdup("");
  }

  if (ast->type == AST_IMPORT && ast->list_value && !ast->alias) {
    str = str_append(&str, ";");

    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      char* name = ast_get_string(child);

      if (!name)
        continue;

      char* defstr;
      TEMPLATE(
        export_def, defstr, (strlen(name) * 2) + (strlen(encoding) * 2), name, encoding, name);

      if (!defstr)
        continue;

      str = str_append(&str, defstr);
      free(defstr);
    }
  }

  char* requirements = 0;
  AST_T* current = ast; // ast->module_root;
  if (current) {
    unsigned int x = 0;
    unsigned int len;
    char** keys;
    map_get_keys(current->requirements, &keys, &len);
    for (unsigned int i = 0; i < len; i++) {
      char* k = keys[i];
      if (!k)
        continue;
      char* reqname = k;

      if (x == 0) {
        requirements = str_append(&requirements, ",/*REQUIREMENTS*/");
      }

      requirements = str_append(&requirements, reqname);

      if (i < len - 1)
        requirements = str_append(&requirements, ",");
      x += 1;
    }
  }

  if (!requirements)
    requirements = strdup("");

  if (!str)
    str = strdup("");

  char* options_str = ast->options ? emit_tuple(ast->options, env) : strdup("");

  char* value;

  char* headers = ast->headers ? strdup(ast->headers) : strdup("");

  if (ast->type == AST_CALL) {
    TEMPLATE(require,
             value,
             strlen(requirements) + (headers ? strlen(headers) : 0) + (k ? strlen(k) : 0),
             headers,
             requirements,
             k,
             options_str);
  } else {
    TEMPLATE(import,
             value,
             (str ? strlen(str) : 0) + (headers ? strlen(headers) : 0) +
               (encoding ? strlen(encoding) : 0) + strlen(options_str) + strlen(requirements) +
               (k ? strlen(k) : 0) + 256,
             headers,
             encoding,
             requirements,
             k,
             options_str,
             str);
  }

  if (!value)
    value = strdup("");

  if (encoding)
    free(encoding);
  if (k)
    free(k);
  if (str)
    free(str);
  if (headers)
    free(headers);

  return value;
}

char* emit_undefined(AST_T* ast, fjb_env_T* env)
{
  return strdup("");
}

char* emit_call(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  if (ast->compiled_value) {
    return emit_import(ast, env);
  }

  if (!ast->left && ast->name) {
    str = str_append(&str, ast->name);
  }

  str = emit_args(ast->list_value, env);

  return str;
}

char* emit_function(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* body_str = 0;
  char* args_str = 0;
  char* expose_str = 0;

  char* name = ast_get_string_copy(ast);

  if (!name) {
    name = strdup(" ");
    /*char buff[256];
    sprintf(buff, "_%p", ast);
    name = strdup(buff);*/
  }

  if (ast->list_value) {
    args_str = emit_args(ast->list_value, env);
  } else {
    args_str = strdup("");
  }

  if (ast->body) {
    body_str = emit(ast->body, env);
  } else {
    body_str = strdup("");
  }

  if (ast->type == AST_CLASS_FUNCTION) {
    TEMPLATE(function_class,
             str,
             strlen(name) + strlen(args_str) + strlen(body_str),
             name,
             args_str,
             body_str);
  } else {
    if (name && !ast->anon && ast->not_exported == 0 &&
        (ast->exported || map_get(env->imports, name))) {
      TEMPLATE(expose_def, expose_str, (strlen(name) * 2) + 16, name, name);
    } else {
      expose_str = strdup("");
    }

    TEMPLATE(function,
             str,
             strlen(name) + strlen(args_str) + strlen(body_str) + strlen(expose_str),
             name,
             args_str,
             body_str,
             expose_str);
  }

  if (body_str)
    free(body_str);
  if (args_str)
    free(args_str);
  if (expose_str)
    free(expose_str);
  if (name)
    free(name);

#ifdef DEBUG
  if (ast->from_module) {
    char buff[256];
    sprintf(buff, "\n/* `%s` */\n", ast->from_module);
    str = str_append(&str, strdup(buff));
  }
#endif

  return str;
}

char* emit_scope(AST_T* ast, fjb_env_T* env)
{
  char* body_str = ast->body ? emit(ast->body, env) : strdup("");

  char* str;
  TEMPLATE(braced, str, strlen(body_str), body_str);

  return str;
}

char* emit_signature(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  if (ast->list_value) {
    char* list_str = emit_args(ast->list_value, env);

    if (list_str) {
      str = str_append(&str, list_str);
      free(list_str);
    }
  }

  if (ast->body) {
    str = str_append(&str, "=>");
    if (ast->body->type == AST_COMPOUND) {
      str = str_append(&str, "{");
      char* body_str = emit(ast->body, env);

      if (body_str) {
        str = str_append(&str, body_str);
        free(body_str);
      }
      str = str_append(&str, "}");
    } else {
      char* body_str = emit(ast->body, env);
      if (body_str) {
        str = str_append(&str, body_str);
        free(body_str);
      }
    }
  }

  return str ? str : strdup("");
}

char* emit_name(AST_T* ast, fjb_env_T* env)
{
  return ast_get_string_copy(ast);
}

char* emit_state(AST_T* ast, fjb_env_T* env)
{
  char* str = strcmp(ast->string_value, "export") == 0 ? 0 : strdup(ast->string_value);

  if (ast->parent && ast->parent->type != AST_SWITCH && strcmp(ast->string_value, "default") == 0) {
    if (str)
      free(str);
    str = 0;
  }

  if (ast->value || ast->right) {
    str = str_append(&str, " ");
    char* valuestr = emit(ast->value ? ast->value : ast->right, env);

    if (valuestr) {
      str = str_append(&str, valuestr);
      free(valuestr);
    }
  }

  return str;
}

char* emit_binop(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  char* leftstr = 0;
  char* rightstr = 0;

  if (ast->left) {
    leftstr = emit(ast->left, env);
  }

  if (ast->right) {
    rightstr = emit(ast->right, env);
  }

  if (!leftstr)
    leftstr = strdup("");
  if (!rightstr)
    rightstr = strdup("");

  if (ast->token) {
    if (ast->token->type == TOKEN_OPTIONAL_CHAIN) {
      TEMPLATE(
        optional_chain, str, (strlen(leftstr) * 3) + (strlen(rightstr) * 3) + 1, leftstr, rightstr);
    } else {
      if (ast->token->type == TOKEN_DOT || ast->token->type == TOKEN_COMMA ||
          ast->token->type == TOKEN_PLUS || ast->token->type == TOKEN_OPTIONAL_CHAIN) {
        str = str_append(&str, leftstr);
        str = str_append(&str, ast->token->value);
        str = str_append(&str, rightstr);
      } else {
        str = str_append(&str, leftstr);
        str = str_append(&str, " ");
        str = str_append(&str, ast->token->value);
        str = str_append(&str, " ");
        str = str_append(&str, rightstr);
      }
    }
  }

  return str ? str : strdup("");
}

char* emit_unop(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->token ? ast->token->value : "");
}

char* emit_block_linked_list(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  if (ast->token && ast->token->value) {
    str = str_append(&str, ast->token->value);
  }

  if (ast->list_value) {
    char* argsstr = emit_args(ast->list_value, env);

    if (argsstr) {
      str = str_append(&str, argsstr);
      free(argsstr);
    }
  }

  if (ast->body) {
    char* bodystr = emit(ast->body, env);
    char* bod;

    if (bodystr) {
      TEMPLATE(braced, bod, strlen(bodystr), bodystr);
      str = str_append(&str, bod);
    }
  }

  return str ? strdup(str) : strdup("");
}

char* emit_increment(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->token->value);
}

char* emit_decrement(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->token->value);
}

char* emit_regex(AST_T* ast, fjb_env_T* env)
{
  return strdup(ast->string_value);
}

char* emit_object(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  str = str_append(&str, "{");

  if (ast->list_value) {
    char* body_str = emit_tuple(ast->list_value, env);

    if (body_str) {
      str = str_append(&str, body_str);
      free(body_str);
    }
  }

  str = str_append(&str, "}");

  return str;
}

char* emit_try(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* linkedstr = emit_block_linked_list(ast, env);

  if (linkedstr) {
    str = str_append(&str, linkedstr);
    free(linkedstr);
  }

  return str ? str : strdup("");
}

char* emit_class(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* name = ast_get_string_copy(ast);
  char* options_str = 0;
  char* bodystr = 0;

  options_str =
    ast->options && ast->options->size ? emit_spaced_list(ast->options, env) : strdup(" ");
  bodystr = ast->body ? emit(ast->body, env) : strdup(" ");

  TEMPLATE(class_definition,
           str,
           strlen(name) + strlen(options_str) + strlen(bodystr),
           name,
           options_str,
           bodystr);

  return str;
}

char* emit_condition(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  if (!ast->body)
    return strdup("");
  str = str_append(&str, ast->expr ? "if" : "");

  if (ast->expr) {
    str = str_append(&str, "(");
    char* exprstr = emit(ast->expr, env);

    if (exprstr) {
      str = str_append(&str, exprstr);
      free(exprstr);
    }
    str = str_append(&str, ")");
  }

  if (ast->body) {

    if ((ast->prev && !ast->prev->dead) || !ast->prev)
      str = str_append(&str, "{");

    char* body_str = emit(ast->body, env);

    if (body_str) {
      str = str_append(&str, body_str);
      free(body_str);
    }

    if ((ast->prev && !ast->prev->dead) || !ast->prev)
      str = str_append(&str, "}");
  }

  if (ast->right) {
    str = str_append(&str, " else ");
  }

  return str;
}

char* emit_switch(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;
  char* expr_str = ast->expr ? emit(ast->expr, env) : strdup("");
  char* body_str = ast->body ? emit(ast->body, env) : strdup("");

  TEMPLATE(switch_statement, str, strlen(expr_str) + strlen(body_str), expr_str, body_str);

  if (expr_str)
    free(expr_str);
  if (body_str)
    free(body_str);

  return str;
}

char* emit_noop(AST_T* ast, fjb_env_T* env)
{
  return strdup("");
}

char* emit_label(AST_T* ast, fjb_env_T* env)
{
  return strdup(":\n");
}

char* emit_ternary(AST_T* ast, fjb_env_T* env)
{
  char* str = 0;

  char* valuestr = emit(ast->value, env);

  if (valuestr) {
    str = str_append(&str, "?");
    str = str_append(&str, valuestr);
    str = str_append(&str, ":");
    free(valuestr);
  }

  return str ? str : strdup("");
}

char* emit_do(AST_T* ast, fjb_env_T* env)
{
  char* str;
  char* bodystr = ast->body ? emit(ast->body, env) : strdup("");

  char* rightstr = ast->right ? emit(ast->right, env) : strdup("");

  if (ast->body && (ast->body->type == AST_COMPOUND || ast->right)) {
    TEMPLATE(do_statement, str, strlen(bodystr) + strlen(rightstr), bodystr, rightstr);
  } else if (ast->body) {
    TEMPLATE(do_statement_no_brace, str, strlen(bodystr), bodystr, rightstr);
  }

  if (bodystr)
    free(bodystr);

  if (rightstr)
    free(rightstr);

  return str;
}
