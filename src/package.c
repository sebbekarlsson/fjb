#include "include/package.h"
#include "include/io.h"
#include "include/string_utils.h"
#include <string.h>

char* package_get(const char* path, const char* key)
{
  char* full_path = 0;
  full_path = str_append(&full_path, path);
  full_path = str_append(&full_path, "/");
  full_path = str_append(&full_path, PACKAGE_NAME);
  // load the parser with the file
  char* contents = fjb_read_file(full_path);
  char* v = 0;

  free(full_path);

  if (!contents)
    return 0;

  json_parser_T* parser = init_json_parser(init_json_lexer(contents));

  // parse the file
  json_ast_T* ast = json_parser_parse(parser);

  // look for the key we are interested in
  for (int i = 0; i < ast->key_value_list_size; i++) {
    json_ast_T* key_value = ast->key_value_list_value[i];

    if (strcmp(key_value->key_value_key, key) == 0) {
      v = strdup(key_value->key_value_value->string_value);
      break;
    }
  }

  // don't forget to free some memory.
  json_parser_free(parser);
  json_ast_free(ast);

  return v;
}
