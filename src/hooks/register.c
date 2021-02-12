#include "../include/hooks/register.h"
#include "../include/hooks/source.h"
#include "../include/hooks/before_import.h"


void register_builtin_hooks()
{
  register_plugin_hook(fjb_hook_source);
  register_plugin_hook(fjb_hook_before_import);
}
