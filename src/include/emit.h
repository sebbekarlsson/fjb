#ifndef FJB_EMIT_H
#define FJB_EMIT_H
#include "AST.h"
char* emit_spaced_list(list_T* list_value);
char* emit_tuple(list_T* list_value);

char* emit(AST_T* ast);
char* emit_compound(AST_T* ast);
char* emit_array(AST_T* ast);
char* emit_tuple_ast(AST_T* ast);
char* emit_hex(AST_T* ast);
char* emit_int(AST_T* ast);
char* emit_bool(AST_T* ast);
char* emit_int_min(AST_T* ast);
char* emit_float(AST_T* ast);
char* emit_string(AST_T* ast);
char* emit_raw(AST_T* ast);
char* emit_template_string(AST_T* ast);
char* emit_arrow_definition(AST_T* ast);
char* emit_assignment(AST_T* ast);
char* emit_colon_assignment(AST_T* ast);
char* emit_case(AST_T* ast);
char* emit_while(AST_T* ast);
char* emit_for(AST_T* ast);
char* emit_compound(AST_T* ast);
char* emit_import(AST_T* ast);
char* emit_undefined(AST_T* ast);
char* emit_call(AST_T* ast);
char* emit_function(AST_T* ast);
char* emit_scope(AST_T* ast);
char* emit_signature(AST_T* ast);
char* emit_name(AST_T* ast);
char* emit_state(AST_T* ast);
char* emit_binop(AST_T* ast);
char* emit_unop(AST_T* ast);
char* emit_increment(AST_T* ast);
char* emit_decrement(AST_T* ast);
char* emit_regex(AST_T* ast);
char* emit_object(AST_T* ast);
char* emit_try(AST_T* ast);
char* emit_class(AST_T* ast);
char* emit_condition(AST_T* ast);
char* emit_switch(AST_T* ast);
char* emit_label(AST_T* ast);
char* emit_ternary(AST_T* ast);
char* emit_noop(AST_T* ast);
char* emit_do(AST_T* ast);
#endif
