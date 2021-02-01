#include "include/compound.h"
#include "include/AST.h"
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

list_T* get_imported_symbols(AST_T* lookup, list_T* imports, list_T* search_index)
{
  list_T* list = FJB_ENV->imported_symbols;

  AST_T* resolved = 0;

  int types[] = { AST_ASSIGNMENT, AST_FUNCTION };
  size_t nr_types = 2;
  query_T data;

  LOOP_NODES(imports, i, child, {
    for (unsigned int k = 0; k < nr_types; k++) {
      data.type = types[k];
      data.name = child->name;
      resolved = ast_query(search_index, resolve_basic_query, data);

      if (resolved && !resolved->is_resolved) {
        /**
         * Copy alias from the import, to the resolved symbol.
         */
        if (child->alias) {
          resolved->alias = strdup(child->alias);
        }

        resolved->is_resolved = 1;
        list_push(list, resolved);
        break;
      }
    }
  });

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
  get_deps(ast->body, args, env);
  get_deps(ast->next, args, env);
  get_deps(ast->value, args, env);
  get_deps(ast->condition, args, env);

  if (ast->list_value && ast->type != AST_FUNCTION) {
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

  if (ptr_in_list(FJB_ENV->imported_symbols, ptr))
    return pushed;

  query_T query;
  query.type = ptr->type;
  query.name = ptr->name;

  // if (!resolve(args.compound, resolve_deps_query, query)) {
  if (args.saved) {
    list_push_at(args.saved, ptr, args.last_pushed ? args.last_pushed : ast);
    args.last_pushed = ptr;
    ptr->is_resolved = 1;
    list_push_safe(FJB_ENV->imported_symbols, ptr);
    pushed += 1;
  }
  //}

  return pushed;
}

AST_T* new_compound(AST_T* lookup, fjb_env_T* env)
{
  if (!env->imports->size)
    return lookup;

  AST_T* compound = init_ast(AST_COMPOUND);
  compound->list_value = list_copy(get_imported_symbols(lookup, env->imports, env->search_index));
  gc_mark_list(env->GC, compound->list_value);

  AST_T* parent = 0;

  if (compound->list_value && compound->list_value->size)
    parent = ((AST_T*)compound->list_value->items[0])->parent;

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

  list_T* all_symbols = list_merge(args.saved, compound->list_value);

  list_T* copied = list_copy(lookup->list_value);
  compound->list_value = all_symbols->size ? all_symbols : copied;

  gc_mark_list(env->GC, copied);
  gc_mark_list(env->GC, all_symbols);

  return compound;
}
