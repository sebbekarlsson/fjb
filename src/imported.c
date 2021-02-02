#include "include/imported.h"
#include <string.h>

imported_T* init_imported(char* name, char* alias, AST_T* ast)
{
  imported_T* imported = calloc(1, sizeof(struct IMPORTED));
  imported->name = strdup(name);
  if (alias)
    imported->alias = strdup(alias);
  imported->ast = ast;

  return imported;
}
