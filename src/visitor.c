#include "include/visitor.h"
#include "include/compound.h"
#include "include/emit.h"
#include "include/emit_hooks.h"
#include "include/env.h"
#include "include/eval.h"
#include "include/eval_jsx.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/id.h"
#include "include/imported.h"
#include "include/io.h"
#include "include/node.h"
#include "include/plugin.h"
#include "include/resolve.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern fjb_env_T* FJB_ENV;

#define IS_TRUE(expr) is_true(visitor, expr)

unsigned int is_true(visitor_T* visitor, AST_T* ast)
{
  AST_T* result = eval(visitor, ast);
  if (!result)
    return 0;

  if (result->bool_value)
    return 1;

  return 0;
}

AST_T* getptr(AST_T* ast, list_T* stack, visitor_T* visitor)
{
  if (!ast)
    return 0;
  if (!ast->name)
    return 0;

  AST_T* ptr = ast->ptr;

  if (ptr && ptr->map && ast->from_obj) {
    AST_T* val = (AST_T*)map_get_value(ptr->map, ast->name);

    if (val && val->type != AST_CALL)
      ptr = val;
  }

  if (!ast->from_obj && !ptr) {
    imported_T* im = fjb_get_imported(ast->name);
    if (im && im->ast) {
      ptr = im->ast;
    }
  }

  if (!ptr && ast->from_obj)
    return 0;

  if (!ast->from_obj && !ptr) {
    ptr = (AST_T*)map_get_value(FJB_ENV->global_imports, ast->name);
    if (!ptr && ast->stack_frame)
      ptr = (AST_T*)map_get_value(ast->stack_frame, ast->name);
    if (!ptr && ast->parent && ast->parent->stack_frame) {
      ptr = (AST_T*)map_get_value(ast->parent->stack_frame, ast->name);
    }
  }

  if (ptr == ast)
    ptr = 0;

  if (ptr && ptr->type == AST_ASSIGNMENT && ptr->value)
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
  unsigned int stack_before = stack->size;
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

  fjb_set_filepath(fjb_call_all_hooks(HOOK_BEFORE_IMPORT, final_file_to_read));

  char* ext = (char*)get_filename_ext(FJB_ENV->filepath);

  compiler_result_T* special = emit_hooks(ext, ast->list_value);

  FJB_ENV->current_import = ast;
  compiler_result_T* result = special ? special : fjb();
  ast->headers = result->headers ? strdup(result->headers) : 0;

  if (!ast->headers && !result->headers) {
    const char* template = "\n/*`%s`*/\n";
    ast->headers = calloc(strlen(final_file_to_read) + strlen(template) + 1, sizeof(char));
    sprintf(ast->headers, template, final_file_to_read);
  }

  ast->compiled_value = strdup(result->stdout);

  if (result->node && !special) {
    AST_T* dot = new_compound(result->node, FJB_ENV);

    ast->node = dot;
    ast->compiled_value = dot->list_value ? emit(dot) : ast->compiled_value;
  }

  if (result->node && special && ast->list_value && ast->list_value->size == 1) {
    AST_T* id = (AST_T*)ast->list_value->items[0];
    map_set(FJB_ENV->global_imports, id->name, result->node);
  }

  fjb_set_filepath(prev_filepath);
  fjb_set_source(prev_source);
  fjb_set_aliased_import(0);

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    AST_T* id = ast->list_value->items[i];
    map_unset(FJB_ENV->imports, id->name);
  }

  if (result)
    compiler_result_free(result);

  unsigned int new_size = stack->size;

  if (ast->name && new_size > stack_before) {
    for (unsigned int i = new_size - 1; i > stack_before; i--) {
      AST_T* child = (AST_T*)stack->items[i];
      if (!child)
        continue;
      if (child == ast)
        continue;

      list_remove(stack, child, 0);

      if (child->name && strcmp(ast->name, child->name) != 0) {
        map_unset(FJB_ENV->map, child->name);
      }
    }
  }

  if (ast->basename) {
    map_set(FJB_ENV->compiled_imports, ast->basename, ast);
    return init_ast(AST_NOOP);
  }

  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  ast->ptr = ast->value;
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, stack);
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->value && ast->left && ast->left->name) {
    ast->value->encoding = strdup(ast->left->name);
    ast->value->bool_value = 1;
  }

  if (!ast->name && ast->left && ast->left->name) {
    ast->name = strdup(ast->left->name);
  }
  if (ast->name && fjb_ast_is_imported(ast)) {
    ast->exported = 1;

    if (ast->flags && ast->flags->size)
      map_set(FJB_ENV->global_imports, ast->name, ast);

    if (ast->value && ast->value->type == AST_FUNCTION) {
      ast->value->bool_value = 1;
      if (ast->value->body)
        ast->value->body->bool_value = 1;
    }
  }

  /* Does not work atm... Incredibly difficult to solve.
   * if (ast->flags && ast->flags->size && ast->name && ast->value && ast->value->type != AST_CALL
   && !resolve_name(ast, "prototype") && !resolve_name(ast, "console")  && !(ast->parent &&
   ast->parent->type == AST_FOR)) {

     if (!ast->from_obj)
        id_generate(ast);

   if (ast->alias && ast->left) ast->left->alias = strdup(ast->alias);
   }*/

  if (ast->value) {
    ast->value->no_ptr = 0;
    ast->value = visitor_visit(visitor, ast->value, stack);
  }

  if (ast->name) {
    imported_T* im = fjb_get_imported(ast->name);

    if (im) {
      im->ast = ast;
      ast->exported = 1;
      map_set(FJB_ENV->global_imports, ast->name, ast);
    } else if (!ast->exported) {
      ast->not_exported = 1;
      if (ast->value)
        ast->value->not_exported = 1;
    }
  }

  if (ast->left && ast->left->name &&
      (strcmp(ast->left->name, "exports") == 0 || strcmp(ast->left->name, "module") == 0)) {
    if (ast->value && ast->value->name) {
      if (ast->value->is_require_call) {
        map_unset(FJB_ENV->compiled_imports, ast->value->encoding);
        AST_T* state = init_ast(AST_STATE);
        state->string_value = strdup("return");
        state->value = ast->value;
        state->ptr = ast->value;
        ast->ptr = ast->value->ptr ? ast->value->ptr : ast->value;
        FJB_ENV->current_import->ptr = ast->ptr;
        // FJB_ENV->current_import = ast->value;
        fjb_register_assignment(ast, ast->name);
        return state;
      }
    }

    if (FJB_ENV->current_import && FJB_ENV->current_import->map && ast->parent &&
        (ast->parent->type == AST_FUNCTION || ast->parent->type == AST_COMPOUND)) {
      if (ast->value)
        map_set(
          FJB_ENV->current_import->map, ast->name, ast->value->ptr ? ast->value->ptr : ast->value);
    }
  }

  if (ast->value && ast->value->type == AST_NOOP)
    return init_ast(AST_NOOP);

  if (ast->left && ast->left->name && ast->value) {
    if (ast->value->is_require_call) {
      if ((!ast->value->compiled_value || ast->value->type == AST_NOOP) &&
          FJB_ENV->current_import) {
        if (!FJB_ENV->current_import->options)
          FJB_ENV->current_import->options = NEW_STACK;

        if (strcmp(ast->left->name, "module") != 0) {
          AST_T* ast_name = init_ast(AST_NAME);
          ast_name->string_value = strdup(ast->left->name);
          list_push(FJB_ENV->current_import->options, ast_name);
          map_set(FJB_ENV->current_import->requirements,
                  ast->left->name,
                  init_assignment(ast->left->name, ast_name));
        }
      }

      ast->value->left = ast;
      // FJB_ENV->current_import = ast->value;
      ast->ptr = ast->value;
      ast->value = 0;
      fjb_register_assignment(ast->ptr, ast->name);
      return init_ast(AST_NOOP);
    }
  }

  if (ast->value && ast->left) {
    AST_T* leftptr = ast->left->ptr;

    AST_T* rightptr = ast->value->ptr ? ast->value->ptr : ast->value;
    char* name = ast->left->type == AST_BINOP ? ast->left->right->name : ast->left->name;

    AST_T* assignment = init_assignment(name, rightptr);
    if (leftptr && leftptr->alias)
      assignment->alias = strdup(leftptr->alias);

    gc_mark(FJB_ENV->GC, assignment);

    list_push(stack, assignment); // TODO: get rid of stack

    if (assignment->name) {
      ast->ptr = rightptr;
      if (ast->value)
        ast->value->ptr = rightptr;
      // map_set(FJB_ENV->map, assignment->name, assignment);
    }

    if (rightptr) {
      // ast->ptr = rightptr;
      if (leftptr && leftptr->writable) {
        if (leftptr->type == AST_OBJECT && leftptr->map && assignment->name) {
          map_set(leftptr->map, assignment->name, assignment->value);
        } else {
          if (leftptr->value) {
            leftptr->value = rightptr;
          } else {
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

  if (ast->flags && ast->flags->size)
    fjb_register_assignment(ast, ast->name);

  return ast;
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, stack);

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
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);

  return ast;
}

AST_T* visitor_visit_scope(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, stack);
  return ast;
}

AST_T* visitor_visit_class(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->options)
    ast->options = visit_array(visitor, ast->options, stack);

  if (ast->name) {
    fjb_register_function(ast, ast->name);
    list_push(stack, ast); // // TODO: get rid of stack

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
  ast->ptr = ast;

  if (ast->name) {
    if (fjb_ast_is_imported(ast)) {
      ast->exported = 1;
      map_set(FJB_ENV->global_imports, ast->name, ast);
      ast->bool_value = 1;
      if (ast->body)
        ast->body->bool_value = 1;
    }
    fjb_register_function(ast, ast->name);
    list_push(stack, ast); // TODO: get rid of stack

    imported_T* im = fjb_get_imported(ast->name);

    if (im) {
      im->ast = ast;
      ast->bool_value = 1;
      map_set(FJB_ENV->global_imports, ast->name, ast);
    } else {
      ast->not_exported = 1;
    }
  }

  map_T* prev = map_copy(FJB_ENV->map);

  if (ast->anon || (ast->parent && ast->parent->type == AST_CALL))
    ast->bool_value = 1;

  if (!ast->anon && !ast->bool_value)
    if (!ast->bool_value && !(ast->body && ast->body->bool_value))
      return ast;
  unsigned int stack_before = stack->size;

  if (ast->list_value && ast->list_value->size) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];

      if (!child)
        continue;

      if (child->type == AST_NAME)
        child->no_ptr = 1;

      ast->list_value->items[i] = visitor_visit(visitor, child, stack);

      if (!ast->body)
        continue;

      char* n = child->name;
      if (!n)
        continue;

      map_set(FJB_ENV->map, n, child);
      list_push(stack, child); // TODO: get rid of stack
    }
  }

  if (ast->body && (ast->bool_value || ast->body->bool_value))
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

      if (child->name && strcmp(ast->name, child->name) != 0) {
        map_unset(FJB_ENV->map, child->name);
        AST_T* prevv = (AST_T*)map_get_value(prev, child->name);

        if (prevv)
          map_set(FJB_ENV->map, child->name, prevv);
      }
    }
  }

  // FJB_ENV->map = prev;
  // map_buckets_apply_source_map(FJB_ENV->map, FJB_ENV->map);

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

      if (child->type == AST_NOOP || child->type == AST_UNDEFINED)
        continue;

      ast->list_value->items[i] = visitor_visit(visitor, child, stack);

      if (child->value && child->type == AST_STATE && child->token &&
          child->token->type == TOKEN_RETURN) {
        ast->ptr = child->ptr ? child->ptr : child;
        if (ast->ptr)
          ast->ptr = ast->ptr->value ? ast->ptr->value : ast->ptr;
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
  if (ast->dead && !ast->ptr)
    return ast;

  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);

  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->ptr)
    ast->from_obj = 1;

  if (!ast->no_ptr)
    ast->ptr = getptr(ast, stack, visitor);

  if (ast->name) {
    AST_T* definition = ast->ptr;

    if (definition && definition->alias && !ast->alias) {
      ast->alias = strdup(definition->alias);
    }
  }

  if (!ast->from_call) {
    AST_T* ptr = ast->ptr;
    if (ptr && !ast_is_callable(ptr))
      ptr = ast_value(ptr);
    ast->ptr = ptr;

    if (ptr && !ast_is_callable(ptr))
      ptr = ast_ptr(ptr);

    if (ast->parent && ast->parent->type == AST_CALL && ptr && ast_is_callable(ptr) &&
        !ptr->bool_value) {
      ptr->bool_value = 1;
      if (ptr->body)
        ptr->body->bool_value = 1;
      ptr = visitor_visit(visitor, ptr, stack);
    }
  }

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

    fjb_register_function(ast, ast->name);
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

  if (ast->left && ast->left->alias)
    ast->alias = strdup(ast->left->alias);

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
    ast->left->from_call = 1;

  if (ast->from_obj && ast->left)
    ast->left->from_obj = 1;

  if (ast->left && !ast->left->ptr && !ast->left->no_ptr)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->ptr)
    ast->ptr = getptr(ast, stack, visitor);

  if (ast->left->ptr && !ast->ptr)
    ast->ptr = ast->left->ptr;

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, stack);

  if ((ast->left && ast->left->type != AST_CALL) &&
      ((ast->left && ast->left->name && strcmp(ast->left->name, "require") == 0) ||
       (ast->left && ast->left->ptr && ast->left->ptr->name &&
        strcmp(ast->left->ptr->name, "require") == 0))) {
    ast->is_require_call = 1;
    if (!ast->map)
      ast->map = NEW_MAP();
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

      if (!ast->encoding)
        ast->encoding = strdup(base);

      AST_T* existing = (AST_T*)map_get_value(FJB_ENV->compiled_imports, ast->encoding);

      if (base &&
          (existing && existing->encoding || (list_contains_str(FJB_ENV->resolved_imports, base) ||
                                              list_contains_str(FJB_ENV->resolved_imports, str)

                                                )) &&
          FJB_ENV->current_import) {
        if (!FJB_ENV->current_import->options)
          FJB_ENV->current_import->options = NEW_STACK;

        return ast;
      }

      list_push(FJB_ENV->resolved_imports, str);

      if (base) {
        list_push(FJB_ENV->resolved_imports, base);
      }

      if (final_file_to_read) {

        char* contents = fjb_read_file(final_file_to_read);
        fjb_set_source(contents);
        char* old = FJB_ENV->filepath;
        FJB_ENV->filepath = strdup(final_file_to_read);
        AST_T* p = FJB_ENV->current_import;
        FJB_ENV->current_import = ast;
        compiler_result_T* result = fjb(FJB_ENV);
        FJB_ENV->current_import = p;

        FJB_ENV->filepath = old;
        ast->compiled_value = strdup(result->stdout);

        ast->headers = result && result->headers ? strdup(result->headers) : strdup("");

        map_set(FJB_ENV->compiled_imports, ast->encoding, ast);

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
      ast->left = 0;
    }
  }

  if (ast->left && ast->left->alias) {
    ast->alias = strdup(ast->left->alias);
  }

  AST_T* ptr = ast->left && ast->left->ptr               ? ast->left->ptr
               : ast->ptr                                ? ast->ptr
               : ast->left && ast_is_callable(ast->left) ? ast->left
                                                         : 0;
  if (ptr == ast)
    ptr = 0;

  if (ptr && !ast_is_callable(ptr)) {
    ptr = ast_ptr(ptr);
  }

  if (ptr && ast_is_callable(ptr)) {
    if ((fjb_ast_is_imported(ptr) ||
         (ptr->name && map_get_value(FJB_ENV->global_imports, ptr->name))) &&
        ptr->body) {
      ptr->bool_value = 0;
      ptr->body->bool_value = 0;
    }
  }

  if (ptr && ast_is_callable(ptr) && !ptr->bool_value) {
    if (ptr->body)
      ptr->body->bool_value = 1;
    ptr->bool_value = 1;

    if (ptr->list_value) {
      for (unsigned int i = 0; i < ast->list_value->size; i++) {
        AST_T* child = (AST_T*)ast->list_value->items[i];
        if (!child)
          continue;

        if (!(i >= ptr->list_value->size) && !(ptr->list_value->size < i)) {
          AST_T* child2 = (AST_T*)ptr->list_value->items[i];
          if (child2 && child2->name) {
            AST_T* v = child->ptr ? child->ptr : child;
            map_set(FJB_ENV->map, child2->name, v);
          }
        }
      }
    }

    visitor_visit(visitor, ptr, stack);

    /*for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child)
        continue;

      if (!(i >= ptr->list_value->size) && !(ptr->list_value->size < i)) {
        AST_T* child2 = (AST_T*)ptr->list_value->items[i];
        if (child2 && child2->name) {
          AST_T* v = ast_ptr(child);
          map_unset(FJB_ENV->map, child2->name);
        }
      }
      }*/
  }
  if (ptr && ptr->body && ptr->body->ptr) {
    ast->ptr = ptr->body->ptr;
  }

  return ast;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->token && ast->token->type == TOKEN_DOT)
    ast->from_obj = 1;
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);

  if (ast->right && (ast->token && ast->token->type == TOKEN_DOT)) {
    ast->right->from_obj = 1;
    ast->from_obj = 1;
  }

  if (ast->left && ast->right && ast->left->ptr && ast->token && ast->token->type == TOKEN_DOT) {
    ast->right->ptr = ast->left->ptr;
    ast->ptr = ast->left->ptr;
  }

  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  if (ast->right && (ast->token && ast->token->type == TOKEN_DOT)) {
    ast->right->from_obj = 1;
    ast->from_obj = 1;
  }

  if (ast->right && ast->right->ptr && ast->token && ast->token->type == TOKEN_DOT) {
    ast->right->from_obj = 1;
    ast->ptr = ast->right->ptr;
  }

  if (ast->token && ast->left && ast->right && ast->token->type == TOKEN_AND_AND) {
    ast->ptr = init_ast(AST_BOOL);
    if (IS_TRUE(ast->left) && IS_TRUE(ast->right)) {
      ast->ptr->bool_value = 1;
    }
  }

  AST_T* result = eval(visitor, ast);
  if (result && result != 0) {
    return result;
  }

  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, stack);

  ast->ptr = ast->right && ast->right->ptr ? ast->right->ptr
             : ast->left && ast->left->ptr ? ast->left->ptr
                                           : 0;
  ast->ptr = init_ast(AST_BOOL);

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

  ast = fjb_call_all_hooks(HOOK_BEFORE_EVAL, ast);

  unsigned int map_size = FJB_ENV->map->nrkeys;

  if (ast && ast->name && (map_size != visitor->map_size || !ast->stack_frame)) {
    if ((FJB_ENV->map && FJB_ENV->map->len_used_buckets && FJB_ENV->map->nrkeys) &&
        !(ast->type == AST_NOOP || ast->type == AST_UNDEFINED || ast->type == AST_INT ||
          ast->type == AST_INT_MIN || ast->type == AST_FLOAT || ast->type == AST_ARROW_DEFINITION ||
          ast->type == AST_CLASS || ast->type == AST_OBJECT || ast->type == AST_COLON_ASSIGNMENT ||
          ast->type == AST_HEX || ast->type == AST_STRING || ast->type == AST_REGEX ||
          ast->type == AST_TUPLE || ast->type == AST_JSX_COMPOUND || ast->type == AST_CONDITION ||
          ast->type == AST_WHILE || ast->type == AST_COMPOUND || ast->type == AST_JSX_ELEMENT ||
          ast->type == AST_JSX_TEMPLATE_STRING || ast->type == AST_FOR || ast->type == AST_DO ||
          ast->type == AST_TRY || ast->type == AST_STATE || ast->type == AST_IMPORT ||
          ast->is_require_call)) {
      /**
       * We really want to call this method as few times as possible.
       * It's very expensive.
       */
      ast->stack_frame = map_copy(FJB_ENV->map);
    }
  }

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
    case AST_UNDEFINED: return ast; break;
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
    case AST_SCOPE: ast = visitor_visit_scope(visitor, ast, stack); break;
    case AST_OBJECT: ast = visitor_visit_object(visitor, ast, stack); break;
    case AST_CONDITION: ast = visitor_visit_condition(visitor, ast, stack); break;
    case AST_SWITCH: ast = visitor_visit_switch(visitor, ast, stack); break;
    case AST_TRY: ast = visitor_visit_try(visitor, ast, stack); break;
    case AST_TERNARY: ast = visitor_visit_ternary(visitor, ast, stack); break;
    case AST_DO: ast = visitor_visit_do(visitor, ast, stack); break;
    case AST_NOOP: return ast; break;
    case AST_TUPLE: ast = visitor_visit_tuple(visitor, ast, stack); break;
    case AST_JSX_TEMPLATE_VALUE: ast = eval_jsx(visitor, ast, stack); break;
    case AST_JSX_COMPOUND: ast = eval_jsx(visitor, ast, stack); break;
    case AST_JSX_ELEMENT: ast = eval_jsx(visitor, ast, stack); break;
    default: {
    } break;
  }

  if (ast->next)
    ast->next = visitor_visit(visitor, ast->next, stack);

  ast = fjb_call_all_hooks(HOOK_AFTER_EVAL, ast);

  visitor->map_size = FJB_ENV->map->nrkeys;

  /*if (ast->ptr && ast->name) {
    printf("<name='%s' type='%s' m='%s'> points to <name='%s' type='%s' m='%s'>\n",
           ast->name,
           AST_TYPE_STR[ast->type],
           ast->from_module ? ast->from_module : "",
           ast->ptr->name ? ast->ptr->name : "?",
           AST_TYPE_STR[ast->ptr->type],
           ast->from_module ? ast->from_module : "");
	   }*/

  return ast;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
