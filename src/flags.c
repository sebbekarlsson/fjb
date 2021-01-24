#include "include/flags.h"
#include <string.h>

compiler_flags_T* init_compiler_flags(char* source, char* filepath, unsigned int should_dump)
{
  compiler_flags_T* flags = calloc(1, sizeof(compiler_flags_T));
  flags->source = strdup(source);
  flags->filepath = strdup(filepath);
  flags->should_dump = should_dump;
  flags->es_exports = NEW_STACK;
  flags->imports = NEW_STACK;
  flags->search_index = NEW_STACK;

  flags->GC = init_gc();

  /* ==== Built-in globals ==== */
  AST_T* module = init_ast(AST_OBJECT);
  module->name = strdup("module");
  module->list_value = init_list(sizeof(AST_T*));
  gc_mark(flags->GC, module);

  AST_T* exports = init_ast(AST_OBJECT);
  exports->list_value = init_list(sizeof(AST_T*));
  exports->name = strdup("exports");
  gc_mark(flags->GC, exports);

  AST_T* assignment = init_ast(AST_ASSIGNMENT);
  assignment->name = strdup("module");
  assignment->value = module;
  gc_mark(flags->GC, assignment);

  AST_T* exports_assignment = init_ast(AST_ASSIGNMENT);
  exports_assignment->name = strdup("exports");
  exports_assignment->value = exports;
  gc_mark(flags->GC, exports_assignment);

  list_push(module->list_value, exports_assignment);

  flags->module = module;
  flags->exports = exports;

  return flags;
}
