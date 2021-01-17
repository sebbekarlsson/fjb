#include "include/visitor.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/node.h"
#include "include/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern gc_T* GC;
extern list_T* defs;

#define IN_IMPORTS(ast)                                                                            \
  ((ast->name && find_in_args(ast->name, visitor->imports)) || ptr_in_list(visitor->imports, ast))

static AST_T* find_in_args(char* name, list_T* args)
{
  if (!name) {
    return 0;
  }

  for (unsigned int i = 0; i < args->size; i++) {
    AST_T* child = (AST_T*)args->items[i];

    if (!child)
      continue;

    if (!child->name) {
      continue;
    }

    if (child->name) {
      if (strcmp(child->name, name) == 0)
        return child;
    }
  }

  return 0;
}

static AST_T* find_in_args_by_type(char* name, int type, list_T* args)
{
  if (!name) {
    return 0;
  }

  for (unsigned int i = 0; i < args->size; i++) {
    AST_T* child = (AST_T*)args->items[i];

    if (!child)
      continue;

    if (child->type != type)
      continue;

    if (!child->name) {
      continue;
    }

    if (child->name) {
      if (strcmp(child->name, name) == 0)
        return child;
    }
  }

  return 0;
}

visitor_T* init_visitor(parser_T* parser, const char* filepath, list_T* refs, list_T* imports,
                        AST_T* module, AST_T* exports, unsigned int all)
{
  visitor_T* visitor = calloc(1, sizeof(struct FJB_VISITOR_STRUCT));
  visitor->parser = parser;
  visitor->filepath = filepath;
  visitor->imports = imports;
  visitor->refs = refs;
  visitor->es_exports = init_list(sizeof(AST_T*));
  visitor->log = init_list(sizeof(AST_T*));
  visitor->module = module;
  visitor->exports = exports;
  visitor->all = all;

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

AST_T* resolve_array_pointer(AST_T* left, AST_T* right)
{
  int index = 0;

  AST_T* val = (AST_T*)right->list_value->items[0];
  if (!val)
    return 0;

  AST_T* rightvalue = val->ptr;

  if (rightvalue && rightvalue->type == AST_INT) {
    index = rightvalue->int_value;
  }

  if (index >= left->list_value->size)
    return 0;

  AST_T* val2 = left->list_value->items[index];
  return val2 ? val2->ptr : 0;
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
  compiler_result_T* result = fjb((GEN_FLAGS){ final_file_to_read, ast->string_value }, contents,
                                  visitor->refs, visitor->imports, 0);
  ast->compiled_value = strdup(result->stdout);
  ast->es_exports = result->es_exports;

  free(final_file_to_read);
  free(contents);

  compiler_result_free(result);

  return ast;
}

static void assign(visitor_T* visitor, AST_T* left, AST_T* value, list_T* args, list_T* stack,
                   AST_T* base_node)
{
  if (left && left->name) {
    AST_T* ptr = left && left->ptr ? left->ptr : find_in_args(left->name, stack);
    AST_T* val = value;

    if (val) {
      visitor_visit(visitor, val, args, stack);
    }

    AST_T* assignment = init_assignment(left->name, val);
    assignment->ptr = base_node;

    AST_T* sym = left;

    if (ptr && ptr->type == AST_OBJECT) {
      if (!ptr->list_value)
        ptr->list_value = NEW_STACK;

      sym = find_in_args(left->name, ptr->list_value);

      if (!sym && ptr->name && strcmp(ptr->name, "exports") == 0) {
        if (val) {
          visitor_visit(visitor, val, args, stack);
        }

        list_push(ptr->list_value, assignment);
      }
    } else {
      list_push_safe(stack, assignment);
    }

    if (sym) {
      left->ptr = value;
      sym->ptr = value;
    }
  }
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left && ast->left->name) {
    AST_T* pending = find_in_args(ast->left->name, args);

    if ((pending && (pending->type == AST_CALL || pending->type == AST_ASSIGNMENT ||
                     pending->type == AST_NAME)) ||
        IN_IMPORTS(ast->left))
      ast->visited = 1;

    if (pending)
      list_remove(args, pending, 0);
  }

  if (ast->ptr)
  {
    visitor_visit(visitor, ast->ptr, args, stack);
  }
  
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack); 
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->left && ast->value) {
    assign(visitor, ast->left, ast->value, args, stack, ast);

    if (ast->value && !ast->ptr)
      ast->ptr = ast->value;
  }

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
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  if (ast->body)
    ast->body = visitor_visit(visitor, ast->body, args, stack);

  if (ast->name) {
    list_push_safe(stack, ast);

    if (IN_IMPORTS(ast))
    {
      list_push_safe(visitor->es_exports, ast);

      if (ast->parent && ast->parent->body)
        ast->parent->body->es_exports = visitor->es_exports;
    }
  }

  ast->visited = (ast->name && IN_IMPORTS(ast)) ? 1 : 0;

  if (!ast->ptr && ast->body)
    ast->ptr = ast->body;

  if (ast->name)
  {
    AST_T* pending = find_in_args(ast->name, args);

    if ((pending && (pending->type == AST_CALL || pending->type == AST_ASSIGNMENT ||
                     pending->type == AST_NAME)) ||
        IN_IMPORTS(ast))
      ast->visited = 1;

    if (pending)
      list_remove(args, pending, 0);
  }
  else
  {
    ast->visited = 1;
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
  unsigned int stack_before = stack->size;

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child)
        continue;

      if (child->parent)
        child->parent->visited = 1;

      ast->list_value->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  if (ast->parent) {
    ast->parent->visited = 1;
  }

  unsigned int new_size = stack->size;

  if (ast->name && ast->parent && new_size > stack_before) {
    for (unsigned int i = new_size - 1; i > stack_before; i--) {
      AST_T* child = (AST_T*)stack->items[i];
      list_remove(stack, child, 0);
    }
  }

  ast->visited = 1;

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
  if (!ast->ptr)
    list_push_safe(args, ast);

  if (ast->left) {
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  }
  if (ast->right) {
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  }

  if (ast->next) {
    ast->next = visitor_visit(visitor, ast->next, args, stack);
  }

  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  if (ast->name && ast->ptr) {
    ast->ptr = resolve_pointer(ast->ptr, ast->name, stack);
  }

  if (!ast->ptr) {
    AST_T* value = find_in_args(ast->name, stack);

    if (value) {
      ast->ptr = value;
    } 
  }

  if (ast->ptr) {
    if (ast->ptr->type != AST_FUNCTION) {
      visitor_visit(visitor, ast->ptr, args, stack);
      ast->ptr->visited = 1;

      if (ast->ptr->source_ast)
        ast->ptr->source_ast->visited = 1;
    }
  } 

  if (ast->ptr && ptr_in_list(args, ast))
  {
    list_remove(args, ast, 0);
  }

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

  if (!ast->ptr)
    if (ast->left && ast->left->ptr)
      ast->ptr = ast->left->ptr;

  if (ast->ptr) {
    visitor_visit(visitor, ast->ptr, args, stack);
    ast->ptr->visited = 1;
    list_push_safe(stack, ast);
    ast->left->visited = 1;
  }
  else if (!ast->from_obj)
  {
    list_push_safe(args, ast);
  }

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
          fjb((GEN_FLAGS){ final_file_to_read, str }, contents, visitor->refs, visitor->imports, 1);
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
  
  ast->visited = 1;

  return ast;
}

AST_T* resolve_pointer(AST_T* ptrin, char* name, list_T* stack)
{
  if (!ptrin)
    return 0;

  AST_T* ptr = ptrin;

  if (ptr) {
    if (ptr->type == AST_OBJECT && ptr->list_value) {
      AST_T* v = find_in_args(name, ptr->list_value);

      if (v) {
        ptr = v;
      } else {
        return ptr;
      }
    }
  }

  return ast_get_final_ptr(ptr);
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);

  if (ast->left && ast->left->ptr) {
    ast->ptr = ast_get_final_ptr(ast->left->ptr);
  }

  if (ast->right && ast->ptr && !ast->right->ptr) {
    ast->right->ptr = ast->ptr;
  }

  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  if (ast->right && ast->right->ptr) {
    ast->ptr = ast->right->ptr;
  }
  
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
  if (ast->list_value && !ptr_in_list(ast->list_value, ast))
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (!ast) {
    printf("[Visitor]: Encountered %p AST.\n", ast);
    exit(1);
  }
  
  if (ast->visited) {
    return ast;
  }
  
  if (ast->next) visitor_visit(visitor, ast->next, args, stack);


  // list_push(visitor->log, ast);

  // if (ast->from_module)
  //  printf("%s\n", ast->from_module);
  // if (ast->line)
  //  printf("%d\n", ast->line);

  if (ast->type == AST_ARRAY && ast->list_value->size == 1 && ast->left &&
      ast->left->type == AST_NAME) {
    AST_T* ptr = ast->left->ptr;

    if (ptr && (ptr->type == AST_ARRAY || ptr->type == AST_TUPLE)) {
      ast->ptr = resolve_array_pointer(ptr, ast);
    }
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

//  if ((ast->type != AST_FUNCTION && ast->type != AST_ASSIGNMENT))
    ast->visited = 1;

 /* if (ast->type == AST_BINOP && ast->right && !ast->right->visited)
  {
    ast->visited = 0;
  }*/

  return ast;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
