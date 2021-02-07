#include "include/visitor.h"
#include "include/compound.h"
#include "include/emit.h"
#include "include/emit_hooks.h"
#include "include/env.h"
#include "include/eval.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/imported.h"
#include "include/io.h"
#include "include/jsx_eval.h"
#include "include/node.h"
#include "include/plugin.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

static AST_T* getptr(AST_T* ast, list_T* stack, visitor_T* visitor)
{
  if (!ast)
    return 0;
  if (!ast->name)
    return 0;

  AST_T* ptr = ast->ptr;

  if (!ptr && ast->stack_frame)
    ptr = (AST_T*)map_get_value(ast->stack_frame, ast->name);

  if (!ptr)
    ptr = (AST_T*)map_get_value(FJB_ENV->map, ast->name);

  if (ptr && ptr->map) {
    AST_T* val = (AST_T*)map_get_value(ptr->map, ast->name);
    if (val)
      ptr = val;
  }

  if (ptr && ptr->value)
    ptr = ptr->value;

  return ptr;
}

visitor_T* init_visitor(parser_T* parser)
{
  visitor_T* visitor = calloc(1, sizeof(struct FJB_VISITOR_STRUCT));
  visitor->parser = parser;

  return visitor;
}

static list_T* visit_array(visitor_T* visitor, list_T* list, list_T* stack)
{
  if (list) {
    for (unsigned int i = 0; i < list->size; i++) {
      AST_T* child = (AST_T*)list->items[i];
      if (!child)
        continue;

      list->items[i] = visitor_visit(visitor, child, stack);
    }
  }

  return list;
}

AST_T* visitor_visit_id(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_float(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_import(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  char* encoding = ast->alias ? strdup(ast->alias) : 0;

  if (!encoding) {
    char buff[156];
    const char* template = "_fjb%d";
    sprintf(buff,
            template,
            (int)(FJB_ENV->level + FJB_ENV->ticks +
                  (int)(FJB_ENV->resolved_imports ? FJB_ENV->resolved_imports->size : 0)));
    encoding = strdup(buff);
  }

  /*
  if (!encoding) {
    char buff[156];
    sprintf(buff, "_%p", ast);
    encoding = strdup(buff);
  }*/

  ast->encoding = encoding;

  char* prev_filepath = strdup(FJB_ENV->filepath);
  char* prev_source = FJB_ENV->source;

  char* final_file_to_read = resolve_import((FJB_ENV->filepath), ast->string_value, 0);
  char* base = final_file_to_read ? get_slashed_path(final_file_to_read) : 0;
  ast->basename = base;

  AST_T* existing = (AST_T*)map_get_value(FJB_ENV->compiled_imports, base);

  if (existing && existing->encoding && FJB_ENV->current_import) {
    FJB_ENV->current_import->options = NEW_STACK;

    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* id = ast->list_value->items[i];
      AST_T* binop = init_ast(AST_BINOP);
      binop->left = init_ast(AST_NAME);
      binop->left->name = strdup(existing->encoding);
      binop->token = init_token(".", TOKEN_DOT);
      binop->right = id;
      list_push(FJB_ENV->current_import->options, binop);
      map_set(FJB_ENV->current_import->requirements, id->name, init_assignment(id->name, binop));
    }

    return init_ast(AST_NOOP);
  }

  if (!final_file_to_read) {
    return init_ast(AST_NOOP);
  }

  if (list_contains_str(FJB_ENV->resolved_imports, final_file_to_read)) {
    return init_ast(AST_NOOP);
  }

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* id = ast->list_value->items[i];
    char* name = id->name;
    char* alias = id->alias;
    map_set(FJB_ENV->imports, name, init_imported(name, alias, 0));
  }

  if (base) {

    if (list_contains_str(FJB_ENV->resolved_imports, base)) {
      return init_ast(AST_NOOP);
    }

    list_push(FJB_ENV->resolved_imports, base);
  }
  list_push(FJB_ENV->resolved_imports, final_file_to_read);

  fjb_set_filepath(final_file_to_read);
  char* contents = fjb_read_file(final_file_to_read);
  fjb_set_source(contents);
  fjb_set_aliased_import(ast->alias != 0);
  free(contents);

  char* ext = (char*)get_filename_ext(FJB_ENV->filepath);
  compiler_result_T* special = emit_hooks(FJB_ENV, ext, ast->list_value);

  FJB_ENV->current_import = ast;
  compiler_result_T* result = special ? special : fjb();
  ast->headers = result->headers ? strdup(result->headers) : 0;

  if (!ast->headers && !result->headers) {
    const char* template = "\n/* IMPORT `%s` */\n";
    ast->headers = calloc(strlen(final_file_to_read) + strlen(template) + 1, sizeof(char));
    sprintf(ast->headers, template, final_file_to_read);
  }

  ast->compiled_value = strdup(result->stdout);

  if (result->node && !special) {
    AST_T* dot = new_compound(result->node, FJB_ENV);

    ast->node = dot;
    ast->compiled_value = dot->list_value ? emit(dot, FJB_ENV) : ast->compiled_value;
  }

  fjb_set_filepath(prev_filepath);
  fjb_set_source(prev_source);
  fjb_set_aliased_import(0);

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* id = ast->list_value->items[i];
    char* name = ast_get_string(id);
    map_unset(FJB_ENV->imports, id->name);
  }

  if (result)
    compiler_result_free(result);

  if (ast->basename) {

    map_set(FJB_ENV->compiled_imports, ast->basename, ast);
    return init_ast(AST_NOOP);
  }

  /* char** keys;
   unsigned int nrkeys;
   map_get_keys(FJB_ENV->imports,  &keys, &nrkeys);
   for (unsigned int i = 0; i < nrkeys; i++) {
     char* key = keys[i];
     if (!key) continue;

     imported_T* imp = map_get_value(FJB_ENV->imports, key);
     if (!imp) continue;
     if (!imp->ast) continue;

     map_unset(FJB_ENV->imports, key);
   }*/

  // list_clear(FJB_ENV->imports);

  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, stack);
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, stack);

  if (ast->left && ast->left->name && ast->value && ast->value->basename && visitor->root) {
    if (ast->value->name && strcmp(ast->value->name, "require") == 0) {
      ast->exported = 1;

      if (!ast->value->compiled_value && FJB_ENV->current_import) {
        if (!FJB_ENV->current_import->options)
          FJB_ENV->current_import->options = NEW_STACK;
        AST_T* ast_name = init_ast(AST_NAME);
        ast_name->string_value = strdup(ast->left->name);
        list_push(FJB_ENV->current_import->options, ast_name);
        map_set(FJB_ENV->current_import->requirements,
                ast->left->name,
                init_assignment(ast->left->name, ast_name));
      }
    }
  }

  if ((ast->value && ast->value->dead) || (ast->left && ast->left->dead) ||
      ast->right && ast->right->dead) {
    ast->type = AST_NOOP;
    ast->left = 0;
    ast->right = 0;
    ast->flags = 0;
    return ast->value;
  }

  if (!ast->name && ast->left && ast->left->name) {
    ast->name = strdup(ast->left->name);
  }

  if (ast->left && ast->left->name && strcmp(ast->left->name, "exports") == 0) {
    if (ast->value && ast->value->type == AST_CALL && ast->value->name) {
      if (strcmp(ast->value->name, "require") == 0) {
        ast = ast->value;
        return ast;
      }
    }
  }

  if (ast->value && ast->left) {
    AST_T* leftptr = ast->left->ptr;
    AST_T* rightptr = ast->value->ptr ? ast->value->ptr : ast->value;
    char* name = ast->left->type == AST_BINOP ? ast->left->right->name : ast->left->name;

    AST_T* assignment = init_assignment(name, rightptr);
    gc_mark(FJB_ENV->GC, assignment);

    list_push(stack, assignment);

    if (assignment->name) {
      map_set(FJB_ENV->map, assignment->name, assignment);
    }

    if (rightptr) {
      if (leftptr && leftptr->writable) {
        if (leftptr->type == AST_OBJECT && leftptr->map && assignment->name) {
          map_set(leftptr->map, assignment->name, assignment->value);
        } else {
          if (leftptr->value)
            leftptr->value = rightptr;
          else {
            ast->left->ptr = rightptr;
          }
        }
      } else {
        if (ast->left->writable) {
          ast->left->ptr = rightptr;
        }
      }
    }
  }

  if (ast->name) {
    imported_T* im = (imported_T*)map_get_value(FJB_ENV->imports, ast->name);

    if (im) {
      im->ast = ast;
    } else if (!ast->exported) {
      ast->not_exported = 1;
      if (ast->value)
        ast->value->not_exported = 1;
    }
  }

  if (ast->name)
    map_set(FJB_ENV->assignments, ast->name, ast);

  return ast;
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->value) {
    ast->value = visitor_visit(visitor, ast->value, stack);
  }

  return ast;
}

AST_T* visitor_visit_try(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_condition(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);

  if (ast->expr && ast->expr->type == AST_BOOL) {
    if (ast->expr->bool_value == 0) {
      ast->body = 0;
      ast->dead = 1;
    } else if (ast->expr->bool_value == 1) {
      return visitor_visit(visitor, ast->body, stack);
    }
  }

  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body && ast->body->type != AST_NOOP)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_do(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_while(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_for(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_switch(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_regex(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_object(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_class(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->options)
    ast->options = visit_array(visitor, ast->options, stack);

  if (ast->name) {
    map_set(FJB_ENV->map, ast->name, ast);
    list_push(stack, ast);
    map_set(FJB_ENV->functions, ast->name, ast);

    imported_T* im = (imported_T*)map_get_value(FJB_ENV->imports, ast->name);

    if (im) {
      im->ast = ast;
    } else {
      ast->not_exported = 1;
    }
  }

  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);

  return ast;
}

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  unsigned int stack_before = stack->size;

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child)
        continue;
      char* n = ast_get_string(child);
      if (!n)
        continue;

      map_set(FJB_ENV->map, n, child);
      list_push(stack, child);
    }
  }

  if (ast->name) {
    map_set(FJB_ENV->map, ast->name, ast);
    list_push(stack, ast);
    map_set(FJB_ENV->functions, ast->name, ast);

    imported_T* im = (imported_T*)map_get_value(FJB_ENV->imports, ast->name);

    if (im) {
      im->ast = ast;
    } else {
      ast->not_exported = 1;
    }
  }

  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);

  unsigned int new_size = stack->size;

  if (ast->name && new_size > stack_before) {
    for (unsigned int i = new_size - 1; i > stack_before; i--) {
      AST_T* child = (AST_T*)stack->items[i];
      if (!child)
        continue;
      if (child == ast)
        continue;

      list_remove(stack, child, 0);

      if (child->name) {
        map_unset(FJB_ENV->map, child->name);
      }
    }
  }

  return ast;
}

AST_T* visitor_visit_signature(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (!visitor->root)
    visitor->root = ast;

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child)
        continue;

      ast->list_value->items[i] = visitor_visit(visitor, child, stack);

      if (child->value && child->type == AST_STATE && child->token &&
          child->token->type == TOKEN_RETURN) {
        ast->ptr = child->value;
      }
    }
  }

  return ast;
}

AST_T* visitor_visit_ternary(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, stack);

  return ast;
}

AST_T* visitor_visit_name(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left) {
    ast->left = visitor_visit(visitor, ast->left, stack);
  }

  if (ast->right) {
    ast->right = visitor_visit(visitor, ast->right, stack);
  }

  ast->ptr = getptr(ast, stack, visitor);

  AST_T* result = eval(visitor, ast);

  return result ? result : ast;
}

AST_T* visitor_visit_arrow_definition(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  if (ast->name) {
    imported_T* im = (imported_T*)map_get_value(FJB_ENV->imports, ast->name);

    if (im) {
      im->ast = ast;
    }
  }

  return ast;
}

AST_T* visitor_visit_colon_assignment(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, stack);
  if (ast->label_value)
    ast->label_value = visitor_visit(visitor, ast->label_value, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, stack);

  if (ast->name) {
    imported_T* im = (imported_T*)map_get_value(FJB_ENV->imports, ast->name);

    if (im) {
      im->ast = ast;
    }
  }

  return ast;
}

AST_T* visitor_visit_increment(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  return ast;
}

AST_T* visitor_visit_decrement(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  return ast;
}

AST_T* visitor_visit_call(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  if ((ast->left && ast->left->type != AST_CALL) &&
      ((ast->left && ast->left->name && strcmp(ast->left->name, "require") == 0) ||
       (ast->left && ast->left->ptr && ast->left->ptr->name &&
        strcmp(ast->left->ptr->name, "require") == 0))) {
    char* str = 0;
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child_ast = (AST_T*)ast->list_value->items[i];

      if (child_ast->type == AST_STRING) {
        if (!is_builtin_module(child_ast->string_value)) {
          str = strdup(child_ast->string_value);
        }
      }
    }

    if (str != 0) {
      char* final_file_to_read = resolve_import(((char*)FJB_ENV->filepath), str, 0);

      if (!final_file_to_read) {
        printf("[Error]: Unable to resolve `%s -> %s`\n", FJB_ENV->filepath, str);
        exit(1);
      }

      char* base = final_file_to_read ? get_slashed_path(final_file_to_read) : 0;
      ast->basename = base ? strdup(base) : 0;

      if ((base && list_contains_str(FJB_ENV->resolved_imports, base)) ||
          list_contains_str(FJB_ENV->resolved_imports, str)) {
        ast->left = 0;
        ast->dead = 1;
        ast->type = AST_NOOP;
        return ast;
      }
      // printf("base:\t\t%s\n", base);

      list_push(FJB_ENV->resolved_imports, str);

      if (base) {
        list_push(FJB_ENV->resolved_imports, base);
      }

      if (final_file_to_read) {

        char* contents = fjb_read_file(final_file_to_read);
        fjb_set_source(contents);
        char* old = FJB_ENV->filepath;
        FJB_ENV->filepath = strdup(final_file_to_read);
        FJB_ENV->current_import = ast;
        compiler_result_T* result = fjb(FJB_ENV);
        FJB_ENV->filepath = old;
        ast->compiled_value = strdup(result->stdout);

        if (result && result->headers)
          ast->headers = strdup(result->headers);
        else if (!result->headers) {
          const char* template = "\n/* REQUIRE `%s` */\n";
          ast->headers = calloc(strlen(final_file_to_read) + strlen(template) + 1, sizeof(char));
          sprintf(ast->headers, template, final_file_to_read);
        }

        fjb_set_filepath(old);
        ast->node = result->node;
        ast->ptr = result->node;
        ast->left = 0;
        free(contents);
        compiler_result_free(result);
        free(final_file_to_read);
      }

      free(str);
    } else {
      ast->dead = 1;
      ast->left = 0;
      return ast;
    }
  }

  return ast;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);

  if (ast->left && ast->left->ptr && ast->right) {
    ast->right->ptr = ast->left->ptr;
    ast->ptr = ast->right->ptr;
  }

  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->right && ast->right->ptr) {
    ast->ptr = ast->right->ptr;
  }

  AST_T* result = eval(visitor, ast);
  if (result && result != 0)
    return result;

  if (ast->right && ast->right->name && !ast->name) {
    ast->name = strdup(ast->right->name);
  }

  return ast;
}

AST_T* visitor_visit_undefined(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_noop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  return ast;
}

AST_T* visitor_visit_hex(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  ast->list_value = visit_array(visitor, ast->list_value, stack);

  return ast;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (!ast) {
    printf("[Visitor]: Encountered %p AST.\n", ast);
    exit(1);
  }

  ast->module_root = visitor->root;

  ast = fjb_call_all_hooks(HOOK_BEFORE_EVAL, ast, FJB_ENV);

  switch (ast->type) {
    case AST_ARRAY: ast = visitor_visit_array(visitor, ast, stack); break;
    case AST_HEX: ast = visitor_visit_hex(visitor, ast, stack); break;
    case AST_INT: ast = visitor_visit_int(visitor, ast, stack); break;
    case AST_FLOAT: ast = visitor_visit_float(visitor, ast, stack); break;
    case AST_STRING: ast = visitor_visit_string(visitor, ast, stack); break;
    case AST_ARROW_DEFINITION: ast = visitor_visit_arrow_definition(visitor, ast, stack); break;
    case AST_ASSIGNMENT: ast = visitor_visit_assignment(visitor, ast, stack); break;
    case AST_COLON_ASSIGNMENT: ast = visitor_visit_colon_assignment(visitor, ast, stack); break;
    case AST_WHILE: ast = visitor_visit_while(visitor, ast, stack); break;
    case AST_FOR: ast = visitor_visit_for(visitor, ast, stack); break;
    case AST_COMPOUND: ast = visitor_visit_compound(visitor, ast, stack); break;
    case AST_IMPORT: ast = visitor_visit_import(visitor, ast, stack); break;
    case AST_UNDEFINED: ast = visitor_visit_undefined(visitor, ast, stack); break;
    case AST_CALL: ast = visitor_visit_call(visitor, ast, stack); break;
    case AST_FUNCTION: ast = visitor_visit_function(visitor, ast, stack); break;
    case AST_CLASS: ast = visitor_visit_class(visitor, ast, stack); break;
    case AST_SIGNATURE: ast = visitor_visit_signature(visitor, ast, stack); break;
    case AST_NAME: ast = visitor_visit_name(visitor, ast, stack); break;
    case AST_STATE: ast = visitor_visit_state(visitor, ast, stack); break;
    case AST_BINOP: ast = visitor_visit_binop(visitor, ast, stack); break;
    case AST_UNOP: ast = visitor_visit_unop(visitor, ast, stack); break;
    case AST_INCREMENT: ast = visitor_visit_increment(visitor, ast, stack); break;
    case AST_DECREMENT: ast = visitor_visit_decrement(visitor, ast, stack); break;
    case AST_REGEX: ast = visitor_visit_regex(visitor, ast, stack); break;
    case AST_OBJECT: ast = visitor_visit_object(visitor, ast, stack); break;
    case AST_CONDITION: ast = visitor_visit_condition(visitor, ast, stack); break;
    case AST_SWITCH: ast = visitor_visit_switch(visitor, ast, stack); break;
    case AST_TRY: ast = visitor_visit_try(visitor, ast, stack); break;
    case AST_TERNARY: ast = visitor_visit_ternary(visitor, ast, stack); break;
    case AST_DO: ast = visitor_visit_do(visitor, ast, stack); break;
    case AST_NOOP: ast = visitor_visit_noop(visitor, ast, stack); break;
    case AST_TUPLE: ast = visitor_visit_tuple(visitor, ast, stack); break;
    case AST_JSX_TEMPLATE_VALUE: ast = eval_jsx(visitor, ast, stack); break;
    case AST_JSX_COMPOUND: ast = eval_jsx(visitor, ast, stack); break;
    case AST_JSX_ELEMENT: ast = eval_jsx(visitor, ast, stack); break;
    default: {
    } break;
  }

  if (ast->next)
    ast->next = visitor_visit(visitor, ast->next, stack);

  if (FJB_ENV->map && !ast->stack_frame) {
    ast->stack_frame = map_copy(FJB_ENV->map);
  }

  ast = fjb_call_all_hooks(HOOK_AFTER_EVAL, ast, FJB_ENV);

  return ast;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
