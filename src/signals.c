#include "include/signals.h"
#include <stdlib.h>

volatile fjb_signals_T* FJB_SIGNALS;

void init_fjb_signals()
{
  FJB_SIGNALS = calloc(1, sizeof(struct FJB_SIGNALS_STRUCT));
  FJB_SIGNALS->imports = NEW_STACK;
  FJB_SIGNALS->imported_symbols = NEW_STACK;
  FJB_SIGNALS->import_nodes = NEW_STACK;
}
