#include "include/visitor.h"
#include "include/node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/gen.h"
#include "include/string_utils.h"
#include "include/fjb.h"
#include "include/io.h"

visitor_T* init_visitor(parser_T* parser)
{
  visitor_T* visitor = calloc(1, sizeof(struct FJB_VISITOR_STRUCT));
  visitor->parser = parser;

  return visitor;
}

static list_T* visit_args(visitor_T* visitor, AST_T* ast, list_T* args) {
  list_T* list_value = init_list(sizeof(AST_T*));

  return list_value;
}

static list_T* visit_tuple(visitor_T* visitor, AST_T* ast, list_T* args) {
  list_T* list_value = init_list(sizeof(AST_T*));

  return list_value;
}

static list_T* visit_semi_args(visitor_T* visitor, AST_T* ast, list_T* args) {
  list_T* list_value = init_list(sizeof(AST_T*));

  return list_value;
}

static list_T* visit_array(visitor_T* visitor, list_T* list, list_T* args) {
  list_T* list_value = init_list(sizeof(AST_T*));

  if (list)
  {
    for (unsigned int i = 0; i < list->size; i++)
    {
      AST_T* child = (AST_T*) list->items[i];

      list_push(list_value, visitor_visit(visitor, child, args));
    }
  }

  return list_value;
}

static AST_T* find_in_args(int type, char* name, list_T* args)
{
  for (unsigned int i = 0; i < args->size; i++)
  {
    AST_T* child = (AST_T*) args->items[i];


    if (type != -1)
    {
      if (child->type != type)
        continue;
    }
    
    if (child->name)
    {
      if (strcmp(child->name, name) == 0)
        return child;
    }
  }

  return 0;
}

AST_T* visitor_visit_id(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_int(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_float(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_import(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* ast, list_T* args) 
{
  AST_T* new_ast = init_ast(AST_ASSIGNMENT);
  new_ast->flags = ast->flags;
  new_ast->ptr = ast->ptr;
  new_ast->name = strdup(ast->name ? ast->name: "");
  new_ast->capsulated = ast->capsulated;
  AST_T* phony = 0;

  if (ast->left && ast->left->name)
  {
    if (ast->value && ast->value->type == AST_CALL && ast->value->left && ast->value->left->name)
    {
      phony = init_ast(AST_IMPORT_PHONY);
      phony->name = strdup(ast->left->name);
    }
    else
    if (ast->value && ast->value->type == AST_BINOP && ast->value->left && ast->value->left->type == AST_CALL)
    {
        if (ast->value->right && ast->value->right->name)
        {
          phony = init_ast(AST_IMPORT_PHONY);
          phony->name = strdup(ast->left->name);
          phony->value = ast->value->right;
        }
    }
  }
  
  if (phony)
  {
    list_push(args, phony);
  } 

  if (ast->left)
    new_ast->left = visitor_visit(visitor, ast->left, args);

  if (ast->list_value && ast->list_value->size)
    new_ast->list_value = visit_array(visitor, ast->list_value, args);

  if (ast->list_value_left && ast->list_value_left->size)
    new_ast->list_value_left = visit_array(visitor, ast->list_value_left, args);

  if (ast->list_value_right && ast->list_value_right->size)
    new_ast->list_value_right = visit_array(visitor, ast->list_value_right, args);

  if (ast->value)
    new_ast->value = visitor_visit(visitor, ast->value, args);

  if (ast->ptr && ast->ptr->type == AST_OBJECT)
  {
    AST_T* obj_val = find_in_args(-1, ast->name, ast->ptr->list_value);

    if (obj_val)
    {
      obj_val->value = new_ast->value;
    }
    else
    {
      list_push(ast->ptr->list_value, new_ast);
    }
  }

  return new_ast; 
}

AST_T* visitor_visit_state(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_STATE);
  new_ast->ptr = ast->ptr;

  if (ast->string_value)
    new_ast->string_value = strdup(ast->string_value);

  if (ast->value)
    new_ast->value = visitor_visit(visitor, ast->value, args);

  return new_ast;
}

AST_T* visitor_visit_try(visitor_T* visitor, AST_T* ast, list_T* args) {
  return ast;
}

AST_T* visitor_visit_condition(visitor_T* visitor, AST_T* ast, list_T* args) {
  AST_T* new_ast = init_ast(AST_CONDITION);
  new_ast->token = ast->token;

  if (ast->expr)
    new_ast->expr = visitor_visit(visitor, ast->expr, args);

  if (ast->body)
    new_ast->body = visitor_visit(visitor, ast->body, args);

  if (ast->left)
    new_ast->left = ast->left;

  if (ast->right)
    new_ast->right = visitor_visit(visitor, ast->right, args);

  return new_ast;
}

AST_T* visitor_visit_do(visitor_T* visitor, AST_T* ast, list_T* args) {
  return ast;
}

AST_T* visitor_visit_while(visitor_T* visitor, AST_T* ast, list_T* args) {
  return ast;
}

AST_T* visitor_visit_for(visitor_T* visitor, AST_T* ast, list_T* args) {
  return ast;
}

AST_T* visitor_visit_switch(visitor_T* visitor, AST_T* ast, list_T* args) {
  return ast;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_ARRAY);
  new_ast->ptr = ast->ptr;
  new_ast->list_value = visit_array(visitor, ast->list_value, args);
  return new_ast;
}

AST_T* visitor_visit_regex(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_object_item(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_object(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_OBJECT);
  new_ast->list_value = visit_array(visitor, ast->list_value, args);
  new_ast->ptr = ast->ptr;
  new_ast->capsulated = ast->capsulated;

  return new_ast;
}

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_FUNCTION);
  new_ast->capsulated = ast->capsulated;
  new_ast->list_value = visit_array(visitor, ast->list_value, args);
  new_ast->ptr = ast->ptr;

  if (ast->body)
  {
    new_ast->body = visitor_visit(visitor, ast->body, args);
  }

  return ast;
}

AST_T* visitor_visit_signature(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_COLON_ASSIGNMENT);
  new_ast->list_value = visit_array(visitor, ast->list_value, args);
  new_ast->ptr = ast->ptr;
  new_ast->capsulated = ast->capsulated;

  if (ast->body)
  {
    new_ast->body = visitor_visit(visitor, ast->body, args);
  }

  return ast;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* compound = init_ast(AST_COMPOUND);
  compound->list_value = visit_array(visitor, ast->list_value, args);
  compound->ptr = ast->ptr;
  compound->capsulated = ast->capsulated;

  return compound;
}

AST_T* visitor_visit_ternary(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* ast_new = init_ast(AST_TERNARY);
  ast_new->capsulated = ast->capsulated;
  if (ast->expr)
    ast_new->expr = visitor_visit(visitor, ast->expr, args);
  if (ast->value)
    ast_new->value = visitor_visit(visitor, ast->value, args);

  ast_new->ptr = ast->ptr;

  return ast;
}

AST_T* visitor_visit_name(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_NAME);
  new_ast->name = strdup(ast->name);
  new_ast->flags = ast->flags;
  new_ast->string_value = new_ast->name;
  new_ast->capsulated = ast->capsulated;
  AST_T* value = find_in_args(AST_ASSIGNMENT, ast->name, args);

  if (ast->ptr)
  {
    AST_T* obj_val = find_in_args(-1, ast->name, ast->ptr->list_value);
    new_ast->ptr = ast->ptr;
  }
  else
  {
    if (value && value->value)
    {
      new_ast->ptr = value->value;
    }
  }

  return new_ast;
}

AST_T* visitor_visit_arrow_definition(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_colon_assignment(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_COLON_ASSIGNMENT);
  new_ast->ptr = ast->ptr;
  new_ast->name = ast->name;
  new_ast->string_value = ast->string_value;
  new_ast->capsulated = ast->capsulated;

  if (ast->left)
  {
    new_ast->left = visitor_visit(visitor, ast->left, args);
  }

  if (ast->expr)
  {
    new_ast->expr = visitor_visit(visitor, ast->expr, args);
  }

  if (ast->value)
  {
    new_ast->value = visitor_visit(visitor, ast->value, args);
  }

  if (ast->right)
  {
    new_ast->right = visitor_visit(visitor, ast->right, args);
  }
  return new_ast;
}

AST_T* visitor_visit_increment(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_decrement(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_label(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_LABEL);
  new_ast->left = visitor_visit(visitor, ast->left, args);
  new_ast->label_value = visitor_visit(visitor, ast->label_value, args);
  new_ast->ptr = visitor_visit(visitor, ast->ptr, args);
  return new_ast;
}

AST_T* visitor_visit_call(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_CALL);
  new_ast->ptr = ast->ptr;
  new_ast->string_value = ast->string_value;
  new_ast->capsulated = ast->capsulated;

  if (ast->left)
    new_ast->left = visitor_visit(visitor, ast->left, args); 

  if (ast->right)
    new_ast->right = visitor_visit(visitor, ast->right, args);

  if (ast->phony_value)
    new_ast->phony_value = ast->phony_value;

  if (ast->list_value)
    new_ast->list_value = visit_array(visitor, ast->list_value, args);

  if (new_ast->left->name && strcmp(new_ast->left->name, "require") == 0)
    {
      char* str = 0;
      for (unsigned int i = 0; i < ast->list_value->size; i++)
      {
        AST_T* child_ast = (AST_T*)ast->list_value->items[i];

        if (child_ast->type == AST_STRING)
        {
          if (!is_builtin_module(child_ast->string_value))
          {
            str = child_ast->string_value;
          }
        }
      }

      if (str != 0)
      {
        char* final_file_to_read = resolve_import((char*) visitor->parser->filepath, str);

        if (final_file_to_read)
        {
          compiler_result_T* result = fjb((GEN_FLAGS){final_file_to_read, str}, fjb_read_file(final_file_to_read));
          ast->compiled_value = result->stdout;
          new_ast->compiled_value = ast->compiled_value;
        }

      }
    }


  if (ast->left)
  {
    new_ast->left = visitor_visit(visitor, ast->left, args);
  }

  return new_ast;
}

AST_T* visitor_visit_binop(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_BINOP);
  new_ast->token = ast->token;
  new_ast->capsulated = ast->capsulated;

  if (ast->phony_value)
    new_ast->phony_value = ast->phony_value;

  AST_T* phony_import = 0;

  if (ast->token && ast->token->type == TOKEN_DOT)
  {  
    if (ast->left && ast->left->name)
    {
      phony_import = find_in_args(AST_IMPORT_PHONY, ast->left->name, args);
    } 
  } 

  if (ast->left)
    new_ast->left = visitor_visit(visitor, ast->left, args); 

  if (ast->right)
    new_ast->right = visitor_visit(visitor, ast->right, args);

  /*if (new_ast->left)
  {
    if (new_ast->left->ptr && new_ast->left->ptr->type == AST_OBJECT)
    {
      AST_T* ptr = new_ast->left->ptr;
      new_ast->right->ptr = ptr;
      new_ast->ptr = ptr; 
    }
  }

  if (new_ast->left)
    new_ast->left = visitor_visit(visitor, new_ast->left, args); 

  if (new_ast->right)
    new_ast->right = visitor_visit(visitor, new_ast->right, args);*/

  if (phony_import && phony_import->value && ast->right && ast->right->type == AST_CALL)
  { 
    if (ast->right->left && ast->right->left->name)
    {
      new_ast->right->left->phony_value = phony_import->value;
    }
  }


  return new_ast;
}

AST_T* visitor_visit_undefined(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_noop(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_unop(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_UNOP);
  new_ast->token = ast->token;
  
  if (ast->left)
  {
    new_ast->left = visitor_visit(visitor, ast->left, args);
  }

  if (ast->right)
  {
    new_ast->right = visitor_visit(visitor, ast->right, args);
  }

  return new_ast;
}

AST_T* visitor_visit_hex(visitor_T* visitor, AST_T* ast, list_T* args)
{
  return ast;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* ast, list_T* args)
{
  AST_T* new_ast = init_ast(AST_TUPLE);

  if (ast->list_value)
    new_ast->list_value = visit_array(visitor, ast->list_value, args);

  return new_ast;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args)
{
  switch (ast->type) {
    case AST_ARRAY: return visitor_visit_array(visitor, ast, args); break;
    case AST_HEX: return visitor_visit_hex(visitor, ast, args); break;
    case AST_INT: return visitor_visit_int(visitor, ast, args); break;
    case AST_FLOAT: return visitor_visit_float(visitor, ast, args); break;
    case AST_STRING: return visitor_visit_string(visitor, ast, args); break;
    case AST_ARROW_DEFINITION: return visitor_visit_arrow_definition(visitor, ast, args); break;
    case AST_ASSIGNMENT: return visitor_visit_assignment(visitor, ast, args); break;
    case AST_COLON_ASSIGNMENT: return visitor_visit_colon_assignment(visitor, ast, args); break;
    case AST_WHILE: return visitor_visit_while(visitor, ast, args); break;
    case AST_FOR: return visitor_visit_for(visitor, ast, args); break;
    case AST_COMPOUND: return visitor_visit_compound(visitor, ast, args); break;
    case AST_IMPORT: return visitor_visit_import(visitor, ast, args); break;
    case AST_UNDEFINED: return visitor_visit_undefined(visitor, ast, args); break;
    case AST_CALL: return visitor_visit_call(visitor, ast, args); break;
    case AST_FUNCTION: return visitor_visit_function(visitor, ast, args); break;
    case AST_SIGNATURE: return visitor_visit_signature(visitor, ast, args); break;
    case AST_NAME: return visitor_visit_name(visitor, ast, args); break;
    case AST_STATE: return visitor_visit_state(visitor, ast, args); break;
    case AST_BINOP: return visitor_visit_binop(visitor, ast, args); break;
    case AST_UNOP: return visitor_visit_unop(visitor, ast, args); break;
    case AST_INCREMENT: return visitor_visit_increment(visitor, ast, args); break;
    case AST_DECREMENT: return visitor_visit_decrement(visitor, ast, args); break;
    case AST_REGEX: return visitor_visit_regex(visitor, ast, args); break;
    case AST_OBJECT: return visitor_visit_object(visitor, ast, args); break;
    case AST_CONDITION: return visitor_visit_condition(visitor, ast, args); break;
    case AST_SWITCH: return visitor_visit_switch(visitor, ast, args); break;
    case AST_TRY: return visitor_visit_try(visitor, ast, args); break;
    case AST_LABEL: return visitor_visit_label(visitor, ast, args); break;
    case AST_TERNARY: return visitor_visit_ternary(visitor, ast, args); break;
    case AST_DO: return visitor_visit_do(visitor, ast, args); break;
    case AST_NOOP: return visitor_visit_noop(visitor, ast, args); break;
    case AST_TUPLE: return visitor_visit_tuple(visitor, ast, args); break;
    default: return ast; break;
  }
}
