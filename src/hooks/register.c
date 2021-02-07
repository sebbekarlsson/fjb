#include "../include/hooks/register.h"
#include "../include/hooks/source.h"


void register_builtin_hooks()
{
  register_plugin_hook(fjb_hook_source);
}
