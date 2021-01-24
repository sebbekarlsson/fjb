#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#include "../../../src/include/AST.h"
void assert_node_exists(AST_T* root, int type, char* name, const char* msg);
void assert_node_not_exists(AST_T* root, int type, char* name, const char* msg);
AST_T* run_get_ast(const char* filepath);
#endif
