#ifndef FJB_PACKAGE_H
#define FJB_PACKAGE_H
#define PACKAGE_NAME "package.json"
#include "../../external/libjson/src/include/json_parser.h"

char* package_get(const char* path, const char* key);
#endif
