#include "include/compound.h"
#include "../external/hashmap/src/include/map.h"
#include "include/AST.h"
#include "include/imported.h"
#include "include/resolve.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
  AST_T* lookup;
  AST_T* compound;
  AST_T* last_pushed;
  AST_T* parent;
  list_T* saved;
} options_T;

extern fjb_env_T* FJB_ENV;

list_T* get_imported_symbols(AST_T* lookup)
{
  list_T* list = NEW_STACK;
  AST_T* resolved = 0;

  int types[] = { AST_ASSIGNMENT, AST_FUNCTION };
  size_t nr_types = 2;
  query_T data;

  char** keys;
  unsigned int len;
  map_get_keys(FJB_ENV->imports, &keys, &len);

  for (unsigned int i = 0; i < len; i++) {
    char* key = keys[i];
    if (!key)
      continue;

    if (list_contains_str(FJB_ENV->resolved_imports, key))
      continue;

    AST_T* child = 0;
    imported_T* imp = (imported_T*)map_get_value(FJB_ENV->imports, key);
    if (!imp)
      continue;

    for (unsigned int k = 0; k < nr_types; k++) {

      data.type = types[k];
      data.name = key;

      if (!resolved) {
        if (types[k] == AST_FUNCTION) {
          resolved = (AST_T*)map_get_value(FJB_ENV->functions, key);
        } else if (types[k] == AST_ASSIGNMENT) {
          resolved = (AST_T*)map_get_value(FJB_ENV->assignments, key);
        }
      }

      if (!resolved)
        continue;

      if (resolved && !resolved->is_resolved) {
        /**
         * Copy alias from the import, to the resolved symbol.
         */
        if (imp && imp->alias) {
          resolved->alias = strdup(imp->alias);
        }

        resolved->is_resolved = 1;
        list_push(list, resolved);
        list_push(FJB_ENV->resolved_imports, key);
        break;
      }
    }
  }

  return list;
}

unsigned int get_deps(AST_T* ast, options_T args, fjb_env_T* env)
{
  if (!ast)
    return 0;
  if (!ast->parent && ast->type != AST_COMPOUND && ast->type != AST_SCOPE)
    return 0;

  if (ast->left && ast->type != AST_CALL)
    get_deps(ast->left, args, env);
  if (ast->right && !ast->right->from_obj)
    get_deps(ast->right, args, env);

  if (ast->expr)
    get_deps(ast->expr, args, env);

  if (ast->body)
    get_deps(ast->body, args, env);

  if (ast->next)
    get_deps(ast->next, args, env);

  if (ast->value)
    get_deps(ast->value, args, env);

  if (ast->condition)
    get_deps(ast->condition, args, env);

  if (ast && ast->list_value && ast->type != AST_FUNCTION && ast->type != AST_IMPORT &&
      ast->type != AST_ARROW_DEFINITION && ast->type != AST_SIGNATURE) {
    LOOP_NODES(
      ast->list_value, i, child, if (child) { get_deps(child, args, env); });
  }

  if (ast && ast->options) {
    LOOP_NODES(
      ast->options, i, child, if (child) { get_deps(child, args, env); });
  }

  AST_T* ptr = 0;

  if (ast->name) {
    query_T query;
    query.name = ast->name;
    query.parent = args.parent;

    int types[] = { AST_ASSIGNMENT, AST_FUNCTION };
    size_t nr_types = 2;

    for (unsigned int i = 0; i < nr_types; i++) {
      query.type = types[i];

      if (types[i] == AST_FUNCTION) {
        ptr = (AST_T*)map_get_value(FJB_ENV->functions, ast->name);
      } else {
        ptr = (AST_T*)map_get_value(FJB_ENV->assignments, ast->name);
        if (ptr && !ptr->flags) {
          ptr = 0;
          continue;
        }
      }

      if (ptr) {
        if (query.parent && ptr->parent && (ptr->parent != query.parent))
          continue;
      }

      if (!ptr)
        continue;

      if (ptr) {
        break;
      }
    }
  }

  unsigned int pushed = 0;

  if (!ptr)
    return pushed;

  if (ptr->is_resolved)
    return pushed;

  if (ptr->parent && ptr->parent->list_value && ptr_in_list(args.saved, ptr->parent)) {
    if (ptr_in_list(ptr->parent->list_value, ptr))
      return pushed;
  }

  if (ptr->name == 0 || (args.saved && ptr_in_list(args.saved, ptr)) || ptr->from_obj ||
      ptr == ast || ptr == args.compound)
    return pushed;

  if (args.saved && ptr && ast) {
    list_push_at(args.saved, ptr, args.last_pushed ? args.last_pushed : ast);
    args.last_pushed = ptr;
    ptr->is_resolved = 1;
    pushed += 1;
  }

  return pushed;
}

AST_T* new_compound(AST_T* lookup, fjb_env_T* env)
{
  if (!env)
    return lookup;

  if (FJB_ENV->level <= 0)
    return lookup;

  AST_T* compound = init_ast(AST_COMPOUND);
  compound->module_root = lookup->module_root;
  list_T* syms = get_imported_symbols(lookup);
  list_T* imported_symbols = syms ? syms : NEW_STACK;
  compound->list_value = imported_symbols;

  AST_T* parent = 0;

  if (imported_symbols && imported_symbols->size)
    parent = ((AST_T*)imported_symbols->items[0])->parent;

  options_T args = {};
  args.lookup = lookup;
  args.saved = compound->list_value;
  args.compound = compound;
  args.parent = parent;

  unsigned int prev = 0;
  unsigned int pushed = 1;

  while ((get_deps(compound, args, env) || (args.saved && args.saved->size != prev)) && pushed) {
    pushed = 0;
    prev = args.saved ? args.saved->size : 0;

    LOOP_NODES_FIXED(args.saved, i, prev, child, { pushed += get_deps(child, args, env); });

    if (!pushed)
      break;
  }

  list_T* all_symbols = args.saved;

  list_T* copied = lookup && lookup->list_value ? list_copy(lookup->list_value) : NEW_STACK;
  compound->list_value = all_symbols->size ? all_symbols : copied;

  gc_mark_list(env->GC, copied);
  gc_mark_list(env->GC, all_symbols);

  return compound;
}
