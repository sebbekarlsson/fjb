#ifndef FJB_GEN_H
#define FJB_GEN_H
#include "AST.h"
#include "env.h"
char* gen_spaced_list(list_T* list_value, fjb_env_T* env);
char* gen_tuple(list_T* list_value, fjb_env_T* env);

char* gen(AST_T* ast, fjb_env_T* env);
char* gen_compound(AST_T* ast, fjb_env_T* env);
char* gen_array(AST_T* ast, fjb_env_T* env);
char* gen_tuple_ast(AST_T* ast, fjb_env_T* env);
char* gen_hex(AST_T* ast, fjb_env_T* env);
char* gen_int(AST_T* ast, fjb_env_T* env);
char* gen_int_min(AST_T* ast, fjb_env_T* env);
char* gen_float(AST_T* ast, fjb_env_T* env);
char* gen_string(AST_T* ast, fjb_env_T* env);
char* gen_raw(AST_T* ast, fjb_env_T* env);
char* gen_template_string(AST_T* ast, fjb_env_T* env);
char* gen_arrow_definition(AST_T* ast, fjb_env_T* env);
char* gen_assignment(AST_T* ast, fjb_env_T* env);
char* gen_colon_assignment(AST_T* ast, fjb_env_T* env);
char* gen_while(AST_T* ast, fjb_env_T* env);
char* gen_for(AST_T* ast, fjb_env_T* env);
char* gen_compound(AST_T* ast, fjb_env_T* env);
char* gen_import(AST_T* ast, fjb_env_T* env);
char* gen_undefined(AST_T* ast, fjb_env_T* env);
char* gen_call(AST_T* ast, fjb_env_T* env);
char* gen_function(AST_T* ast, fjb_env_T* env);
char* gen_scope(AST_T* ast, fjb_env_T* env);
char* gen_signature(AST_T* ast, fjb_env_T* env);
char* gen_name(AST_T* ast, fjb_env_T* env);
char* gen_state(AST_T* ast, fjb_env_T* env);
char* gen_binop(AST_T* ast, fjb_env_T* env);
char* gen_unop(AST_T* ast, fjb_env_T* env);
char* gen_increment(AST_T* ast, fjb_env_T* env);
char* gen_decrement(AST_T* ast, fjb_env_T* env);
char* gen_regex(AST_T* ast, fjb_env_T* env);
char* gen_object(AST_T* ast, fjb_env_T* env);
char* gen_try(AST_T* ast, fjb_env_T* env);
char* gen_class(AST_T* ast, fjb_env_T* env);
char* gen_condition(AST_T* ast, fjb_env_T* env);
char* gen_switch(AST_T* ast, fjb_env_T* env);
char* gen_label(AST_T* ast, fjb_env_T* env);
char* gen_ternary(AST_T* ast, fjb_env_T* env);
char* gen_noop(AST_T* ast, fjb_env_T* env);
char* gen_do(AST_T* ast, fjb_env_T* env);
#endif
