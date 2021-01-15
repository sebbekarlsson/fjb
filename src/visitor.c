#include "include/visitor.h"
#include "include/fjb.h"
#include "include/gc.h"
#include "include/gen.h"
#include "include/io.h"
#include "include/node.h"
#include "include/string_utils.h"
#include "include/mkalive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern gc_T* GC;
extern list_T* defs;

#define IN_IMPORTS(ast)\
  (in_symbols(ast, visitor->imports) || ptr_in_list(visitor->imports, ast))

static AST_T* find_in_args(int type, char* name, list_T* args)
{
  if (!name)
    return 0;

  for (unsigned int i = 0; i < args->size; i++) {
    AST_T* child = (AST_T*)args->items[i];

    if (!child)
      continue;

    if (type != -1) {
      if (child->type != type)
        continue;
    }

    if (!child->name)
      continue;

    if (child->name) {
      if (strcmp(child->name, name) == 0)
        return child;
    }
  }

  return 0;
}

static list_T* get_deps(visitor_T* visitor)
{
  list_T* deps = NEW_STACK;

  for (unsigned int i = 0; i < visitor->exports->list_value->size; i++)
  {
    AST_T* child = (AST_T*) visitor->exports->list_value->items[i];

    if (!ptr_in_list(deps, child))
      list_push(deps, child);

    list_T* pointers = ast_get_pointers(child);

    for (unsigned int j = 0; j < pointers->size; j++)
    {
      AST_T* ptr = (AST_T*) pointers->items[i];

      if (!ptr_in_list(deps, ptr))
        list_push(deps, ptr);
    }

    if (pointers)
    {
      if (pointers->items)
        free(pointers->items);

      free(pointers);
    }
  }

  return deps;
}

unsigned int alive_in_list(list_T* list)
{
  for (unsigned int i = 0; i < list->size; i++) {
    AST_T* child = list->items[i];
    if (child->alive) return 1;
  }

  return 0;
}

static unsigned int in_symbols(AST_T* ast, list_T* symbols)
{
  char* in_name = ast->name ? ast->name : ast->string_value;

  if (!in_name)
    return 0;

  for (unsigned int i = 0; i < symbols->size; i++) {
    AST_T* child = symbols->items[i];

    char* child_name = child->name ? child->name : child->string_value;

    if (!child_name)
      continue;

    if (strcmp(in_name, child_name) == 0)
      return 1;
  }

  return 0;
}

static AST_T* find_symbol(AST_T* ast, list_T* symbols)
{
  char* in_name = ast->name ? ast->name : ast->string_value;

  if (!in_name)
    return 0;

  for (unsigned int i = 0; i < symbols->size; i++) {
    AST_T* child = symbols->items[i];

    char* child_name = child->name ? child->name : child->string_value;

    if (!child_name)
      continue;

    if (strcmp(in_name, child_name) == 0)
      return child;
  }

  return 0;
}

unsigned int is_called(AST_T* ast, list_T* stack)
{
  if (!ast) return 0;
  if (!ast->name) return 0;

  AST_T* sym = find_in_args(AST_CALL, ast->name, stack);

  return sym ? 1 : 0;
}

static unsigned int is_dead(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
 if (!ast) return 0;

 if (ast->name && (ast->type == AST_FUNCTION || ast->type == AST_DEFINITION))
  {
    if ((ast->type == AST_FUNCTION && is_called(ast, stack)) || IN_IMPORTS(ast))
    {
      return 0; 
    }
    else
    {
      return 1;
    }
  }

 return 0;
}

static unsigned int is_completely_dead(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  unsigned int dead = 0;

  if (in_symbols(ast, stack)) return 0;

  return is_dead(visitor, ast, args, stack) || 
         ast->ptr ? is_dead(visitor, ast->ptr, args, stack) : 0 ||
         ast->value ? is_dead(visitor, ast->value, args, stack) : 0;
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
      if (!child) continue;

      list->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  return list;
}

AST_T* get_pointer(AST_T* ptr, unsigned int body)
{
  if (!ptr)
    return 0;

  if (ptr->list_value && ptr->list_value->size == 1 && ptr->type != AST_ARRAY) {
    AST_T* child = (AST_T*)ptr->list_value->items[0];
    if (child->type == AST_TUPLE)
      return child;
  }
  
  if (ptr->type == AST_FUNCTION) return ptr;
  if (ptr->type == AST_BINOP && ptr->right) return get_pointer(ptr->right, body);
  if (ptr->type == AST_FUNCTION && body && ptr->body) return get_pointer(ptr->body, body); 
  if (ptr->value) return get_pointer(ptr->value, body);
  if (ptr->ptr && ptr->ptr != ptr) return get_pointer(ptr->ptr, body);

  return ptr;
}

AST_T* resolve_array_pointer(AST_T* left, AST_T* right)
{
  int index = 0;

  AST_T* rightvalue = get_pointer((AST_T*)right->list_value->items[0], 0);

  if (rightvalue && rightvalue->type == AST_INT) {
    index = rightvalue->int_value;
  }

  if (index >= left->list_value->size)
    return 0;

  return (AST_T*)get_pointer(left->list_value->items[index], 0);
}

AST_T* visitor_visit_id(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_float(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_import(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);

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

static void assign(visitor_T* visitor, AST_T* left, AST_T* value, list_T* args, list_T* stack)
{
  if (left && left->name) {
    AST_T* ptr = get_pointer(left, 0);
    AST_T* val = value;
    AST_T* assignment = init_assignment(left->name, val);
    assignment->ptr = val;

    if (ptr)
      ALIVE(ptr);

    AST_T* sym = left;

    if (ptr && ptr->type == AST_OBJECT)
    {
      if (!ptr->list_value) ptr->list_value = NEW_STACK;

      sym = find_in_args(AST_ASSIGNMENT, left->name, ptr->list_value);

      if (!sym && ptr->name && strcmp(ptr->name, "exports") == 0)
      {
        if (val)
        {
          visitor_visit(visitor, val, args, stack);
        }

        if (val)
        {
          val->alive = 0;
          ALIVE(val);
        }

        list_push(ptr->list_value, assignment);
      }
    }
    else
    {
      list_push_safe(stack, assignment);
    }
    
    if (sym)
    {
      left->ptr = value;
      sym->ptr = value;
    }
  }
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);  

  if (ast->left && ast->value)
  {
    assign(visitor, ast->left, most_right_value(ast->value), args, stack);
    
    if (ast->value)
      ast->ptr = ast->value;
  }

  if (ast->left && ast->left->alive) ast->alive = 1; 
  if (ast->right && ast->right->alive) ast->alive = 1; 
  if (ast->value && ast->value->alive) ast->alive = 1; 
  if (ast->expr && ast->expr->alive) ast->alive = 1; 

  if (ast->left) ALIVE(ast->left);
  if (ast->right) ALIVE(ast->right);
  
  list_push_safe(stack, ast);

  if (ast->value && ast->value->alive)
  {  
    ast->alive = 0;
    ALIVE(ast);
  }
  else
  {
    ast->alive = 0;
  }

  return ast;
}

AST_T* visitor_visit_definition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->expr)
    ast->expr = visitor_visit(visitor, ast->expr, args, stack);
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  if (ast->left && ast->left->alive) ast->alive = 1;
  else if (ast->right && ast->right->alive) ast->alive = 1;
  else if (ast->value && ast->value->alive) ast->alive = 1;
  else if (ast->expr && ast->expr->alive) ast->alive = 1;

  return ast;
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->value)
    ast->ptr = ast->value; 

  if (ast->value)
    ast->value->source_ast = ast;

  if (ast->value && ast->value->value) {
    ast->value->value->source_ast = ast;
  }

  if (strcmp(ast->name, "export") == 0 && ast->value) {
    if (ast->value->alive)
      list_push_safe(visitor->es_exports, ast->value);

    if (ast->value && ast->ptr && ast->ptr != ast->value)
    {
      if (!ast->ptr->alive)
        visitor_visit(visitor, ast->ptr, args, stack);
    }
  }

  if (ast->parent && ast->parent->type == AST_FUNCTION && !ast->parent->ptr)
  {
    ast->parent->ptr = ast->ptr;
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

  ALIVE(ast);

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

  ALIVE(ast);

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

  ALIVE(ast);

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

  ALIVE(ast);

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

  ALIVE(ast);

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

  ALIVE(ast);
  
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

  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_regex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_object(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  visit_array(visitor, ast->list_value, args, stack);  

  if (!ptr_in_list(stack, ast))
  {
    if (ast->name) {
      list_push(stack, ast);
    }
  }
  
  if (ast->body)
    visitor_visit(visitor, ast->body, args, stack);

  if (IN_IMPORTS(ast)) ALIVE(ast);

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

  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  unsigned int stack_before = stack->size;
  list_T* ignored = NEW_STACK;

  if (ast->list_value) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];
      if (!child) continue;

      if ((child->parent && child->parent->type == AST_FUNCTION && is_completely_dead(visitor, child, args, stack)))
      {
        child->alive = 0;
        list_push_safe(stack, child);
        list_push_safe(ignored, child);
      }
      else if (((child->type == AST_FUNCTION && !child->name) || in_symbols(child, stack)) && child->type != AST_DEFINITION)
      {
        child->alive = 0;

        if (child->type == AST_FUNCTION)
          ALIVE(child);
      }
      
      ast->list_value->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }
  
  unsigned int new_size = stack->size;

  if (ast->type == AST_COMPOUND && ast->name && new_size > stack_before && ast->name)
  {
    for (unsigned int i = new_size-1; i > stack_before; i--)
    {
      AST_T* child = (AST_T*) stack->items[i];
      if (ptr_in_list(ignored, child)) continue;

      list_remove(stack, child, 0);
    }
  }

  if (ignored)
  {
    if (ignored->items) free(ignored->items);
    free(ignored);
  }

  return ast;
}

AST_T* visitor_visit_ternary(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  
  if (!ast->left->alive)
    ALIVE(ast->left);

  if (!ast->right->alive)
    ALIVE(ast->right);

  if (!ast->alive)
    ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_name(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->def && ast->def->alive) return ast;
  if (ast->def && !ast->def->alive) ALIVE(ast->def);

  if (ast->left)
  {
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  }
  if (ast->right)
  {
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  }

  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  AST_T* value = find_in_args(AST_ASSIGNMENT, ast->name, args);

  if (!ast->ptr && value) {
    ast->ptr = value;
  }

  if (ast->from_obj == 0) {
    AST_T* value = find_in_args(-1, ast->name, stack);

    if (value && value != ast && value != ast->ptr)
    {
      if (value && value->alive == 0)
        ALIVE(value);

      ast->ptr = value;
      
      visitor_visit(visitor, ast->ptr, args, stack);
    }
  }
  else if (ast->from_obj && ast->ptr && ast->name)
  {
    ast->ptr = resolve_pointer(ast->ptr, ast->name, stack);
  }

  if (IN_IMPORTS(ast)) 
  {
    ast->alive = 0;
    ALIVE(ast);
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

  ALIVE(ast);

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

  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_increment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_decrement(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  
  ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_call(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{ 
  if (ast->left) ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right) ast->right = visitor_visit(visitor, ast->right, args, stack);

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  AST_T* ptr = ast->left ? ast_search_pointer(ast->left, AST_FUNCTION) : 0;

  if (ptr)
  {
    ast->ptr = ptr;
    
    ast->ptr->alive = 0;
    ALIVE(ptr);

    visitor_visit(visitor, ptr, args, stack);
  }

  if (ast->left && ast->left->name)
  {
    AST_T* def = find_in_args(-1, ast->left->name, stack);
    if (!ast->def) ast->def = def;

    if (def)
    {
      visitor_visit(visitor, def, args, stack);
    }

    if (def)
    {
      def->alive = 0;
      ALIVE(def);
    }
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
        ast->alive = 1;
        // gc_mark(GC, ast->left);
        ast->left = 0;
        free(contents);
        compiler_result_free(result);
        free(final_file_to_read);
      }

      free(str);
    }
  }
  
  ast->alive = 0;
  ALIVE(ast);

  list_push_safe(stack, ast);
  
  return ast;
}

AST_T* resolve_pointer(AST_T* ptrin, char* name, list_T* stack)
{
  AST_T* ptr = get_pointer(ptrin, 1);

  if (ptr) {
    if (ptr->type == AST_OBJECT && ptr->list_value) {
      AST_T* v = find_in_args(-1, name, ptr->list_value);

      if (v)
        return v;

      return ptr;
    }

    if (ptr->value)
      ptr = get_pointer(ptr->value, 1);
  }

  return ptr;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->left)
  {
    if (!ast->left->visited)
    {
      ast->left = visitor_visit(visitor, ast->left, args, stack);
    }

    ast->left->alive = 1;
  
    if (ast->left->ptr && !ast->ptr)
      ast->ptr = ast->left->ptr;
  }
  
  if (!ast->right->visited)
  {
    ast->right = visitor_visit(visitor, ast->right, args, stack);
    ast->right->visited = 1;
  }

  if (ast->right && ast->right->name)
  {
    ast->ptr = resolve_pointer(ast->ptr, ast->right->name, stack);
  } 
   
  if (ast->right && ast->token && ast->token->type == TOKEN_AND_AND && !ast->ptr)
  {
    ast->ptr = ast->right->ptr;
  }

  if (ast->right->name)
  {
    ast->name = strdup(ast->right->name);
  }

  if (ast->token && ast->token->type == TOKEN_DOT && ast->ptr)
  {
    if (ast->right && !ast->right->ptr)
    {
      ast->right->ptr = ast->ptr;
    }
  }

  if (ast->ptr)
  {
    ALIVE(ast->ptr);
  }
  
  ast->right->alive = 0; 
  ALIVE(ast->right);

  if (ast->left->alive)
    ast->right->alive = 1;
  else if (ast->right->alive)
    ast->left->alive = 1;

  return ast;
}

AST_T* visitor_visit_undefined(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_noop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  
  if (!ast->alive) 
    ALIVE(ast);

  return ast;
}

AST_T* visitor_visit_hex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ALIVE(ast);
  return ast;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

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
  
  if (visitor->all)
    ast->alive = 1;

  if (ast->visited || ptr_in_list(visitor->log, ast)) return ast;

  list_push(visitor->log, ast);
  
  //if (ast->from_module)
  //  printf("%s\n", ast->from_module);
  //if (ast->line)
  //  printf("%d\n", ast->line); 

  if (ast->type == AST_ARRAY && ast->list_value->size == 1 && ast->left &&
      ast->left->type == AST_NAME) {
    AST_T* ptr = get_pointer(ast->left, 0);

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
    case AST_ARROW_DEFINITION: ast = visitor_visit_arrow_definition(visitor, ast, args, stack); break;
    case AST_ASSIGNMENT: ast = visitor_visit_assignment(visitor, ast, args, stack); break;
    case AST_DEFINITION: ast = visitor_visit_definition(visitor, ast, args, stack); break;
    case AST_COLON_ASSIGNMENT: ast = visitor_visit_colon_assignment(visitor, ast, args, stack); break;
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
      ALIVE(ast);
      /* noop */
    } break;
  }

  ast->visited = 1;

  return ast;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
