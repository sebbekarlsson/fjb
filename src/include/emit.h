#ifndef FJB_GEN_H
#define FJB_GEN_H
#include "AST.h"
#include "env.h"
char* emit_spaced_list(list_T* list_value, fjb_env_T* env);
char* emit_tuple(list_T* list_value, fjb_env_T* env);

char* emit(AST_T* ast, fjb_env_T* env);
char* emit_compound(AST_T* ast, fjb_env_T* env);
char* emit_array(AST_T* ast, fjb_env_T* env);
char* emit_tuple_ast(AST_T* ast, fjb_env_T* env);
char* emit_hex(AST_T* ast, fjb_env_T* env);
char* emit_int(AST_T* ast, fjb_env_T* env);
char* emit_bool(AST_T* ast, fjb_env_T* env);
char* emit_int_min(AST_T* ast, fjb_env_T* env);
char* emit_float(AST_T* ast, fjb_env_T* env);
char* emit_string(AST_T* ast, fjb_env_T* env);
char* emit_raw(AST_T* ast, fjb_env_T* env);
char* emit_template_string(AST_T* ast, fjb_env_T* env);
char* emit_arrow_definition(AST_T* ast, fjb_env_T* env);
char* emit_assignment(AST_T* ast, fjb_env_T* env);
char* emit_colon_assignment(AST_T* ast, fjb_env_T* env);
char* emit_case(AST_T* ast, fjb_env_T* env);
char* emit_while(AST_T* ast, fjb_env_T* env);
char* emit_for(AST_T* ast, fjb_env_T* env);
char* emit_compound(AST_T* ast, fjb_env_T* env);
char* emit_import(AST_T* ast, fjb_env_T* env);
char* emit_undefined(AST_T* ast, fjb_env_T* env);
char* emit_call(AST_T* ast, fjb_env_T* env);
char* emit_function(AST_T* ast, fjb_env_T* env);
char* emit_scope(AST_T* ast, fjb_env_T* env);
char* emit_signature(AST_T* ast, fjb_env_T* env);
char* emit_name(AST_T* ast, fjb_env_T* env);
char* emit_state(AST_T* ast, fjb_env_T* env);
char* emit_binop(AST_T* ast, fjb_env_T* env);
char* emit_unop(AST_T* ast, fjb_env_T* env);
char* emit_increment(AST_T* ast, fjb_env_T* env);
char* emit_decrement(AST_T* ast, fjb_env_T* env);
char* emit_regex(AST_T* ast, fjb_env_T* env);
char* emit_object(AST_T* ast, fjb_env_T* env);
char* emit_try(AST_T* ast, fjb_env_T* env);
char* emit_class(AST_T* ast, fjb_env_T* env);
char* emit_condition(AST_T* ast, fjb_env_T* env);
char* emit_switch(AST_T* ast, fjb_env_T* env);
char* emit_label(AST_T* ast, fjb_env_T* env);
char* emit_ternary(AST_T* ast, fjb_env_T* env);
char* emit_noop(AST_T* ast, fjb_env_T* env);
char* emit_do(AST_T* ast, fjb_env_T* env);
#endif
