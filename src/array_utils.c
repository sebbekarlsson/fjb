#include "include/array_utils.h"

unsigned int int_in_array(int x, int arr[], unsigned int length)
{
  for (unsigned int i = 0; i < length; i++) {
    if (x == arr[i])
      return 1;
  }

  return 0;
}
