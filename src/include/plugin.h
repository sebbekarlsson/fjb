#ifndef FJB_PLUGIN_H
#define FJB_PLUGIN_H
#include "env.h"

enum PLUGIN_HOOKS
{
  HOOK_BEFORE_EVAL,
  HOOK_AFTER_EVAL,

  HOOK_BEFORE_GENERATE,
  HOOK_AFTER_GENERATE,

  HOOK_BEFORE_COMPILE
};

typedef void* (*plugin_hook)(int action, void* ptr, fjb_env_T* env);

void register_plugin_hook(plugin_hook hook);

void* call_plugin_hook(plugin_hook hook, int action, void* ptr, fjb_env_T* env);

void load_plugins();
#endif
