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

static unsigned int is_alive(AST_T* ast)
{
  if (ast->list_value)
  {
    for (unsigned int i = 0; i < ast->list_value->size; i++)
    {
      AST_T* child = ast->list_value->items[i];
      if (is_alive(child)) return 1;
    }
  }

  return ast->alive;
}

list_T* alives;
static void make_alive(AST_T* ast)
{
  if(!alives) alives = NEW_STACK;

  if (alives && ptr_in_list(alives, ast)) return;

  if (ast->list_value)
  {
    for (unsigned int i = 0; i < ast->list_value->size; i++)
    {
      AST_T* child = ast->list_value->items[i];
      make_alive(child);
    }
  }
  
  if (ast->ptr && !ast->ptr->alive) make_alive(ast->ptr);

  if (ast->value)
  {
    make_alive(ast->value);
  }

  if (ast->left)
  {
    make_alive(ast->left);
  }

  if (ast->right)
  {
    make_alive(ast->right);
  }

  if (ast->body)
  {
    make_alive(ast->body);
  }

  if (ast->expr)
  {
    make_alive(ast->expr);
  }

  if (ast->def)
  {
    ast->def->alive = 1;
  }

  list_push(alives, ast);

  ast->alive = 1;
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
  visitor->local_refs = init_list(sizeof(AST_T*));
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
      child->alive = 1;

      list->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  return list;
}

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

AST_T* get_pointer(AST_T* ptr, unsigned int body)
{
  if (!ptr)
    return 0;

  if (ptr->list_value && ptr->list_value->size == 1 && ptr->type != AST_COMPOUND) {
    AST_T* child = (AST_T*)ptr->list_value->items[0];
    if (child->type == AST_TUPLE)
      return child;
  }
  
  if (body && ptr->body) return get_pointer(ptr->body, body); 
  if (ptr->value) return get_pointer(ptr->value, body);
  if (ptr->ptr && ptr->ptr != ptr) return get_pointer(ptr->ptr, body);

  return ptr;
}

AST_T* resolve_array_pointer(AST_T* left, AST_T* right)
{
  int index = 0;

  AST_T* rightvalue = get_pointer((AST_T*)right->list_value->items[0], 0);

  if (rightvalue->type == AST_INT) {
    index = rightvalue->int_value;
  }

  if (index >= left->list_value->size)
    return 0;

  return (AST_T*)get_pointer(left->list_value->items[index], 0);
}

AST_T* visitor_visit_id(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_float(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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
  ast->alive = 1;

  free(final_file_to_read);
  free(contents);

  compiler_result_free(result);

  return ast;
}

static void assign(AST_T* left, AST_T* value)
{
  if (left) {
    AST_T* ptr = get_pointer(left->ptr, 1);
    AST_T* val = get_pointer(value, 0);

    if (ptr && !ptr->name) return;
    if (ptr && ptr->name && !(strcmp(ptr->name, "exports") == 0)) return;

    if (val && ptr && ptr->type == AST_OBJECT && ptr->list_value && left->name && val != ptr && val != left) {
      AST_T* assignment = init_assignment(left->name, val);
      list_push(ptr->list_value, assignment);
    }
    else 
    if (val && left) {
      left->ptr = val;
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

  if (ast->left && (in_symbols(ast->left, visitor->imports) || ast->left->alive))
    make_alive(ast);
  
  if (ast->left)
  {
    assign(ast->left, get_pointer(ast->value, 0));
    
    if (ast->left->ptr)
      ast->ptr = ast->left->ptr;
  }

  if (!ptr_in_list(stack, ast))
  {
    list_push(stack, ast);
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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->value)
    ast->value = visitor_visit(visitor, ast->value, args, stack);
  if (ast->value)
    ast->ptr = get_pointer(ast->value, 1);

  if (strcmp(ast->name, "export") == 0) {
    if (ast->value && ast->value->alive) {
      list_push(visitor->es_exports, ast->value);
    }
  }

  if (ast->value)
    ast->value->source_ast = ast;

  if (ast->value && ast->value->value) {
    ast->value->value->source_ast = ast;
  }

  if (ast->value && ast->value->alive == 0) {
    ast->alive = visitor->all;
  }

  return ast;
}

AST_T* visitor_visit_try(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_condition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_do(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_while(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_for(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

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

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_switch(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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

  make_alive(ast);
  return ast;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit_regex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_object(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  make_alive(ast);
  return ast;
}

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if ((in_symbols(ast, visitor->imports) || in_symbols(ast, visitor->local_refs) || !ast->name) ||
      visitor->all)
    ast->alive = 1;

  if (!ptr_in_list(stack, ast))
  {
    if (ast->name) {
      list_push(stack, ast);
    }
  }

  if ((ast->alive && ast->source_ast) || (ast->source_ast && visitor->all))
    ast->source_ast->alive = 1;

  if ((ast->alive || ast->exported || visitor->all) && ast->body)
  {
    visitor_visit(visitor, ast->body, args, NEW_STACK);
  }

  return ast;
}

AST_T* visitor_visit_signature(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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
  ast->alive = 1;

  return ast;
}

AST_T* visitor_visit_ternary(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_name(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);
  if (ast->flags)
    ast->flags = visit_array(visitor, ast->flags, args, stack);

  if (!ast->ptr) {
    AST_T* value = find_in_args(AST_ASSIGNMENT, ast->name, args);

    if (value)
      ast->ptr = value;
  }

  if (ast->from_obj == 0) {
    AST_T* value = find_in_args(-1, ast->name, stack);

    if (value)
    {
      ast->ptr = get_pointer(value, 0);
    }
    
    list_push(visitor->local_refs, ast);
  }
  else if (ast->from_obj && ast->ptr && ast->name)
  {
    ast->ptr = resolve_pointer(ast->ptr, ast->name, stack);
  }

  if (ast->ptr)
    make_alive(ast->ptr);

  /*if (ast->name && strcmp(ast->name, "e") == 0)
  {
    printf("--------------%s-------------\n", ast->name);
    printf("%d %s\n", ast->from_obj, ast_to_str(ast->ptr));
    printf("----------------------------\n");
  }*/

  return ast;
}

AST_T* visitor_visit_arrow_definition(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
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
  ast->alive = 1;
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

  make_alive(ast);
  return ast;
}

AST_T* visitor_visit_increment(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  make_alive(ast);
  return ast;
}

AST_T* visitor_visit_decrement(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  make_alive(ast);
  return ast;
}

AST_T* visitor_visit_call(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{ 
  if (ast->left) ast->left = visitor_visit(visitor, ast->left, args, stack);

  if (ast->list_value)
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);
  
  if (ast->left)
    list_push(visitor->local_refs, ast->left);

  ast->alive = 1;

  if (ast->left) {
    AST_T* tmp = ast->left->ptr;
    
    AST_T* ptr = get_pointer(ast->left, 0);

    if (ptr) {

      if (ptr->body)
      {
        visitor_visit(visitor, ptr, args, stack);
      }

      ast->ptr = ptr;
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
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  
    if (ast->left->ptr)
      ast->ptr = get_pointer(ast->left, 1);

    //if (ast->left->name)
    //  ast->name = strdup(ast->left->name);
  }

  if (ast->right && ast->right->name)
  {
    ast->right->ptr = resolve_pointer(ast->ptr, ast->right->name, stack);
    ast->ptr = ast->right->ptr;
  }
   
  ast->right = visitor_visit(visitor, ast->right, args, stack);

  return ast;
}

AST_T* visitor_visit_undefined(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_noop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (ast->left)
    ast->left = visitor_visit(visitor, ast->left, args, stack);
  if (ast->right)
    ast->right = visitor_visit(visitor, ast->right, args, stack);

  make_alive(ast);

  return ast;
}

AST_T* visitor_visit_hex(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;
  return ast;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  ast->alive = 1;

  if (!in_symbols(ast, ast->list_value))
    ast->list_value = visit_array(visitor, ast->list_value, args, stack);

  return ast;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack)
{
  if (!ast) {
    printf("[Visitor]: Encountered %p AST.\n", ast);
    exit(1);
  }

  ast->visited = 1;

  if (visitor->all)
    ast->alive = 1;
  
  //if (ast->from_module)
  //  printf("%s\n", ast->from_module);
  //if (ast->line)
  //  printf("%d\n", ast->line);

  if (ast->list_value && ast->type == AST_COMPOUND) {
    for (unsigned int i = 0; i < ast->list_value->size; i++) {
      AST_T* child = (AST_T*)ast->list_value->items[i];

      if (child->type == AST_STATE) {
        if (ast != child)
          visitor_visit(visitor, child, args, stack);

        if (strcmp(child->name, "return") == 0) {
          if (child->value)
          {
            ast->ptr = get_pointer(child, 1);
            if (child == ast->ptr)
              ast->ptr = 0;
          }
        }
      }
      
      ast->list_value->items[i] = visitor_visit(visitor, child, args, stack);
    }
  }

  if (ast->type == AST_ARRAY && ast->list_value->size == 1 && ast->left &&
      ast->left->type == AST_NAME) {
    AST_T* ptr = get_pointer(ast->left, 0);

    if (ptr && (ptr->type == AST_ARRAY || ptr->type == AST_TUPLE)) {
      ast->ptr = resolve_array_pointer(ptr, ast);
    }
  }

  if (ast->ptr)
    ast->ptr->alive = 1;

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
      /* noop */
    } break;
  }

  ast->visited = 1;
}

void visitor_free(visitor_T* visitor)
{
  free(visitor);
}
