#include "include/visitor.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/node.h"
#include "include/resolve.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern gc_T* GC;
extern list_T* defs;

visitor_T* init_visitor(parser_T* parser, const char* filepath, list_T* imports,
                        AST_T* module, AST_T* exports)
{
  visitor_T* visitor = calloc(1, sizeof(struct FJB_VISITOR_STRUCT));
  visitor->parser = parser;
  visitor->filepath = filepath;
  visitor->imports = imports;
  visitor->es_exports = init_list(sizeof(AST_T*));
  visitor->module = module;
  visitor->exports = exports;

  return visitor;
}

static list_T* visit_array(visitor_T* visitor, list_T* list, list_T* args, list_T* stack)
{
  if (list) {
    for (unsigned int i = 0; i < list->size; i++) {
      AST_T* child = (AST_T*)list->items[i];
      if (!child)
        continue;

      list->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  return list;
}

AST_T* visitor_visit_id(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_float(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_import(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  char* final_file_to_read = resolve_import((char*)visitor->filepath, ast->string_value);

  for (unsigned int i = 0; i < ast->list_value->size; i++) {
    list_push(visitor->imports, ast->list_value->items[i]);
  }

  char* contents = fjb_read_file(final_file_to_read);
  compiler_result_T* result = fjb((GEN_FLAGS){ final_file_to_read, ast->string_value }, contents, visitor->imports);
  ast->compiled_value = strdup(result->stdout);
  ast->es_exports = result->es_exports;

  free(final_file_to_read);
  free(contents);

  compiler_result_free(result);

  list_clear(visitor->imports);

  return ast;
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{ 
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);

  if (!ast->name && ast->left && ast->left->name)
    ast->name = strdup(ast->left->name);

  return ast;
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (strcmp(ast->name, "export") == 0 && ast->value) {
      list_push_safe(visitor->es_exports, ast->value);
  }

  if (ast->value)
  {
    visitor_visit(visitor, ast->value, args, stack);
  }

  return ast;
}

AST_T* visitor_visit_try(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_condition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
   
  return ast;
}

AST_T* visitor_visit_do(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_while(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_for(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_switch(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_regex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_object(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  unsigned int stack_before = stack->size;

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  if (ast->list_value)
  {
    for (unsigned int i = 0; i < ast->list_value->size; i++)
    {
      AST_T* child = (AST_T*) ast->list_value->items[i];
    }
  }

  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack); 

  unsigned int new_size = stack->size;

  if (ast->name && new_size > stack_before) {
    for (unsigned int i = new_size-1; i > stack_before; i--) {
      AST_T* child = (AST_T*)stack->items[i];
      if (!child) continue;

      list_remove(stack, child, 0);
    }
  }

  return ast;
}

AST_T* visitor_visit_signature(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child)
        continue; 
      
      ast->list_value->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  return ast;
}

AST_T* visitor_visit_ternary(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  
  return ast;
}

AST_T* visitor_visit_name(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left) {
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  }
  if (ast->right) {
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  }

  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_arrow_definition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_colon_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->label_value)
    ast->label_value = visitor_visit(visitor, ast->label_value, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  
  return ast;
}

AST_T* visitor_visit_increment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  
  return ast;
}

AST_T* visitor_visit_decrement(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  
  return ast;
}

AST_T* visitor_visit_call(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  if (ast->left && ast->left->name && strcmp(ast->left->name, "require") == 0) {
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
      char* final_file_to_read = resolve_import((char*)visitor->parser->filepath, str);

      if (final_file_to_read) {
        char* contents = fjb_read_file(final_file_to_read);
        compiler_result_T* result =
          fjb((GEN_FLAGS){ final_file_to_read, str }, contents, visitor->imports);
        ast->compiled_value = strdup(result->stdout);
        ast->compiled_value = ast->compiled_value;
        // gc_mark(GC, ast->left);
        ast->left = 0;
        free(contents);
        compiler_result_free(result);
        free(final_file_to_read);
      }

      free(str);
    }
  }


  list_push_safe(stack, ast);
  
  return ast;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);

  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  return ast;
}

AST_T* visitor_visit_undefined(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_noop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  return ast;
}

AST_T* visitor_visit_hex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  return ast;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (!ast) {
    printf("[Visitor]: Encountered %p AST.\n", ast);
    exit(1);
  }


  switch (ast->type) {
    case AST_ARRAY: ast = visitor_visit_array(visitor, ast, args, stack); break;
    case AST_HEX: ast = visitor_visit_hex(visitor, ast, args, stack); break;
    case AST_INT: ast = visitor_visit_int(visitor, ast, args, stack); break;
    case AST_FLOAT: ast = visitor_visit_float(visitor, ast, args, stack); break;
    case AST_STRING: ast = visitor_visit_string(visitor, ast, args, stack); break;
    case AST_ARROW_DEFINITION:
      ast = visitor_visit_arrow_definition(visitor, ast, args, stack);
      break;
    case AST_ASSIGNMENT: ast = visitor_visit_assignment(visitor, ast, args, stack); break;
    case AST_COLON_ASSIGNMENT:
      ast = visitor_visit_colon_assignment(visitor, ast, args, stack);
      break;
    case AST_WHILE: ast = visitor_visit_while(visitor, ast, args, stack); break;
    case AST_FOR: ast = visitor_visit_for(visitor, ast, args, stack); break;
    case AST_COMPOUND: ast = visitor_visit_compound(visitor, ast, args, stack); break;
    case AST_IMPORT: ast = visitor_visit_import(visitor, ast, args, stack); break;
    case AST_UNDEFINED: ast = visitor_visit_undefined(visitor, ast, args, stack); break;
    case AST_CALL: ast = visitor_visit_call(visitor, ast, args, stack); break;
    case AST_FUNCTION: ast = visitor_visit_function(visitor, ast, args, stack); break;
    case AST_SIGNATURE: ast = visitor_visit_signature(visitor, ast, args, stack); break;
    case AST_NAME: ast = visitor_visit_name(visitor, ast, args, stack); break;
    case AST_STATE: ast = visitor_visit_state(visitor, ast, args, stack); break;
    case AST_BINOP: ast = visitor_visit_binop(visitor, ast, args, stack); break;
    case AST_UNOP: ast = visitor_visit_unop(visitor, ast, args, stack); break;
    case AST_INCREMENT: ast = visitor_visit_increment(visitor, ast, args, stack); break;
    case AST_DECREMENT: ast = visitor_visit_decrement(visitor, ast, args, stack); break;
    case AST_REGEX: ast = visitor_visit_regex(visitor, ast, args, stack); break;
    case AST_OBJECT: ast = visitor_visit_object(visitor, ast, args, stack); break;
    case AST_CONDITION: ast = visitor_visit_condition(visitor, ast, args, stack); break;
    case AST_SWITCH: ast = visitor_visit_switch(visitor, ast, args, stack); break;
    case AST_TRY: ast = visitor_visit_try(visitor, ast, args, stack); break;
    case AST_TERNARY: ast = visitor_visit_ternary(visitor, ast, args, stack); break;
    case AST_DO: ast = visitor_visit_do(visitor, ast, args, stack); break;
    case AST_NOOP: ast = visitor_visit_noop(visitor, ast, args, stack); break;
    case AST_TUPLE: ast = visitor_visit_tuple(visitor, ast, args, stack); break;
    default: {
    } break;
  } 

  if (ast->next) visitor_visit(visitor, ast->next, args, stack);

  return ast;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
