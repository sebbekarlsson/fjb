#include "include/string_utils.h"
#include "include/package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* str_append(char** source, const char* piece)
{

  char* src = *source;

  if (!piece)
    return src;

  if (!src) {
    src = calloc((strlen(piece) + 1) * sizeof(char), sizeof(char));
  } else {
    src = realloc(src, (strlen(src) + strlen(piece) + 1) * sizeof(char));
  }
  strcat(src, piece);

  return src;
}

char* str_prefix(char* source, const char* piece)
{
  if (!piece)
    return source;

  char* str = calloc(strlen(source) + strlen(piece) + 1, sizeof(char));
  sprintf(str, "%s%s", piece, source);

  return str;
}

char* str_encode(char* source)
{
  if (!source)
    return 0;

  char* newstr = 0;
  size_t len = strlen(source);

  if (len) {
    newstr = str_append(&newstr, "_");
  }

  for (unsigned int i = 0; i < len; i++) {
    char buff[4];
    sprintf(buff, "%d", (int)source[i]);

    newstr = str_append(&newstr, buff);
  }

  return newstr;
}

char* charstr(char c)
{
  char* str = calloc(2, sizeof(char));
  str[0] = c;
  str[1] = '\0';

  return str;
}

char* dirname(const char* path)
{
  char* last = strrchr(path, '/');
  int pos = (last)-path;
  char* dir = strndup(path, pos);

  if (dir)
  {
    if (strrchr(dir, '/') == 0)
    {
      free(dir);
      dir = strdup(".");
    }
  }

  return dir;
}

const char* extension(const char* path)
{
  char* last = strrchr(path, '.');
  int pos = last - path;
  return path + pos;
}

char* remove_char(char* str, char find)
{
  char* newstr = calloc(1, sizeof(char));

  for (int i = 0; i < strlen(str); i++) {
    if (str[i] != find) {
      newstr = realloc(newstr, (strlen(newstr) + 2) * sizeof(char));
      char* chstr = charstr(str[i]);
      strcat(newstr, chstr);
      free(chstr);
    }
  }

  return newstr;
}

const char* get_filename_ext(const char* filename)
{
  const char* dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

char* int_to_str(int x)
{
  char* str = calloc(32, sizeof(char));
  sprintf(str, "%d", x);

  return str;
}

char* float_to_str(float x)
{
  char* str = calloc(32, sizeof(char));
  sprintf(str, "%12.6f", x);

  return str;
}

unsigned int is_special(char c)
{
  const char* specials = "\"!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
  return strchr(specials, c) != (void*)0;
}

unsigned int first_char_is_special(char* str)
{
  if (!str)
    return 0;

  unsigned int length = strlen(str);
  if (!length)
    return 0;

  return is_special(str[0]);
}

char* resolve_import(char* basepath, char* filepath)
{
  if (!filepath) {
    return 0;
  }
  const char* ext = extension(basepath);
  char* file_to_read = strdup(filepath);

  char* dir = 0;

  if (file_to_read[0] == '.') {
    dir = dirname(basepath);
  } else {
    dir = str_append(&dir, "node_modules");
    dir = str_append(&dir, "/");
    dir = str_append(&dir, file_to_read);

    char* package_json_main = package_get(dir, "main");

    if (file_to_read) {
      free(file_to_read);
    }

    file_to_read = package_json_main ? package_json_main : strdup("index");
  }

  if (!strlen(get_filename_ext(file_to_read))) {
    file_to_read = str_append(&file_to_read, ext);
  }

  dir = str_append(&dir, "/");

  char* final_file_to_read = str_prefix(file_to_read, dir);

  free(file_to_read);
  free(dir);

  return final_file_to_read;
}

char* remove_whitespace(char* source)
{
  char* newstr = 0;
  size_t len = strlen(source);

  for (unsigned int i = 0; i < len; i++)
  {
    char c = source[i];
    
    if (
      c == '\n' ||
      c == '\t' ||
      c == '\r' ||
      c == 10 ||
      c == ' '
    )
      continue;

    newstr = str_append(&newstr, (char[]){c, 0});
  }

  return newstr;
}
