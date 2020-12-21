#include "include/io.h"
#include "include/fjb.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  compiler_result_T* result = fjb((GEN_FLAGS){argv[1], 0, 0}, fjb_read_file(argv[1]));

  printf("%s\n", result->stdout);

  return 0;
}
