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

static unsigned int resolve_basic_query(AST_T* ast, query_T data)
{
  if (!ast)
    return 0;
  if (data.name == 0)
    return 0;

  unsigned int type_match = (ast->type == data.type) || data.type == -1;
  unsigned int name_match =
    ast->name && (ast->name == data.name || (strcmp(ast->name, data.name) == 0));

  return type_match && name_match;
}

static unsigned int resolve_deps_query(AST_T* ast, query_T data)
{
  if (!ast)
    return 0;
  if (ast->from_obj)
    return 0;
  if (!ast->name)
    return 0;
  if (data.type == AST_ASSIGNMENT && !ast->flags)
    return 0;
  if (data.parent && ast->parent && (ast->parent != data.parent) &&
      (ast->parent->type == AST_FUNCTION))
    return 0;

  return resolve_basic_query(ast, data);
}

list_T* get_imported_symbols(AST_T* lookup, list_T* search_index)
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
    AST_T* child = 0;
    imported_T* imp = (imported_T*)map_get_value(FJB_ENV->imports, key);
    if (!imp)
      continue;

    for (unsigned int k = 0; k < nr_types; k++) {
      data.type = types[k];
      data.name = key;

      // if (imp->ast) resolved = imp->ast;

      if (!resolved) {
        if (types[k] == AST_FUNCTION) {
          resolved = (AST_T*)map_get_value(FJB_ENV->functions, key);
        } else if (types[k] == AST_ASSIGNMENT) {
          resolved = (AST_T*)map_get_value(FJB_ENV->assignments, key);
        }
      }

      if (!resolved)
        resolved = ast_query(search_index, resolve_basic_query, data);

      if (resolved && !resolved->is_resolved) {
        /**
         * Copy alias from the import, to the resolved symbol.
         */
        if (imp && imp->alias) {
          resolved->alias = strdup(imp->alias);
        }

        if (imp) {
          // imp->ast = resolved;
        }

        resolved->is_resolved = 1;
        list_push(list, resolved);
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

  get_deps(ast->expr, args, env);
  // get_deps(ast->node, args, env);
  get_deps(ast->body, args, env);
  get_deps(ast->next, args, env);
  get_deps(ast->value, args, env);
  get_deps(ast->condition, args, env);

  if (ast->list_value && ast->type != AST_FUNCTION && ast->type != AST_IMPORT &&
      ast->type != AST_ARROW_DEFINITION && ast->type != AST_SIGNATURE) {
    LOOP_NODES(ast->list_value, i, child, get_deps(child, args, env););
  }

  if (ast->options) {
    LOOP_NODES(ast->options, i, child, get_deps(child, args, env););
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

      // TODO: make this work
      if (types[i] == AST_FUNCTION) {
        ptr = (AST_T*)map_get_value(FJB_ENV->functions, ast->name);
      } else {
        ptr = (AST_T*)map_get_value(FJB_ENV->assignments, ast->name);
        if (ptr && !ptr->flags)
          ptr = 0;
      }

      // if (ptr && ptr->type != types[i]) ptr = 0;

      if (ptr) {
        if (query.parent && ptr->parent && (ptr->parent != query.parent) &&
            (ptr->parent->type == AST_FUNCTION))
          continue;
      }

      if (!ptr)
        ptr = ast_query(env->search_index, resolve_deps_query, query);

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

  if (ptr->name == 0 || (args.saved && ptr_in_list(args.saved, ptr)) || ptr->from_obj ||
      ptr == ast || ptr == args.compound)
    return pushed;

  // if (ptr_in_list(FJB_ENV->imported_symbols, ptr))
  //  return pushed;

  query_T query;
  query.type = ptr->type;
  query.name = ptr->name;

  if (!resolve(args.compound, resolve_deps_query, query)) {
    if (args.saved) {
      list_push_at(args.saved, ptr, args.last_pushed ? args.last_pushed : ast);
      args.last_pushed = ptr;
      ptr->is_resolved = 1;
      //  list_push_safe(FJB_ENV->imported_symbols, ptr);
      pushed += 1;
    }
  }

  return pushed;
}

AST_T* new_compound(AST_T* lookup, fjb_env_T* env)
{
  if (FJB_ENV->level <= 0)
    return lookup;

  AST_T* compound = init_ast(AST_COMPOUND);
  list_T* syms = get_imported_symbols(lookup, env->search_index);
  list_T* imported_symbols = syms ? list_copy(syms) : NEW_STACK;
  compound->list_value = imported_symbols;
  // gc_mark_list(env->GC, compound->list_value);

  AST_T* parent = 0;

  if (imported_symbols && imported_symbols->size)
    parent = ((AST_T*)imported_symbols->items[0])->parent;

  options_T args;
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

  list_T* all_symbols = args.saved; // list_merge( compound->list_value,args.saved);

  list_T* copied = lookup && lookup->list_value ? list_copy(lookup->list_value) : NEW_STACK;
  compound->list_value = all_symbols->size ? all_symbols : copied;

  // gc_mark_list(env->GC, copied);
  gc_mark_list(env->GC, all_symbols);

  return compound;
}
