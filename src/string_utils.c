#include "include/string_utils.h"
#include "include/io.h"
#include "include/package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

char* str_prefix(char** source, const char* piece)
{
  char* v = strdup(*source);

  const char* template = "%s%s";
  char* newstr = calloc((strlen(v) + strlen(piece) + strlen(template)), sizeof(char));
  sprintf(newstr, template, piece, v);

  v = newstr;
  *source = v;

  return v;
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
  if (!path)
    return 0;

  char* last = strrchr(path, '/');
  if (!get_filename_ext((char*)path) && last)
    return strdup(path);

  int pos = (last)-path;
  char* dir = strndup(path, pos);

  if (get_filename_ext((char*)dir) && !last)
    return strdup(".");

  return dir ? dir : strdup(path);
}

const char* extension(const char* path)
{
  char* last = strrchr(path, '.');

  if (!last || !path)
    return 0;

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

const char* get_filename_ext(char* filename)
{
  if (!filename)
    return 0;

  char* f = filename;

  char* last = strrchr(f, '.');
  if (!last)
    return 0;

  return last;
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

int is_dir(const char* path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

char* strip_ext(char* filepath)
{
  char* fname = strdup(filepath);

  char* end = fname + strlen(fname);

  while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
    --end;
  }

  if ((end > fname && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
    *end = '\0';
  }

  return fname;
}

char* get_basename(char* filepath)
{
  size_t len = strlen(filepath);

  if (filepath[len - 1] == '/')
    return strdup(filepath);
  if (is_dir(filepath)) {
    char* str = 0;
    str = str_append(&str, filepath);
    str = str_append(&str, "/");
    return str;
  }

  return strip_ext(filepath);
}

char* get_entry(char* dir)
{
  char* entry_point = 0;

  if (!entry_point)
    entry_point = package_get(dir, "main");

  if (!entry_point) {
    entry_point = package_get(dir, "jsnext:main");
  }

  return entry_point;
}

char* try_resolve(char* path)
{
  const char* extensions[] = { ".js", ".ts", ".jsx" };
  size_t nr_extensions = 3;

  for (unsigned int i = 0; i < nr_extensions; i++) {
    char buff[360];
    sprintf(buff, "%s%s", path, extensions[i]);

    if (file_exists(buff) && !is_dir(buff))
      return strdup(buff);
  }

  return 0;
}

char* resolve_import(char* basepath, char* filepath, unsigned int node_modules)
{
  char* path = 0;
  char* full_path = 0;

  basepath = dirname(basepath);

  char* with_e = try_resolve(filepath);
  if (with_e)
    return with_e;

  if (filepath[0] != '.') {
    basepath = str_append(&basepath, "/node_modules");
  }

  full_path = str_append(&full_path, basepath);
  full_path = str_append(&full_path, "/");
  full_path = str_append(&full_path, filepath);

  if (is_dir(full_path)) {
    char* entry = get_entry(full_path);

    if (entry) {
      full_path = str_append(&full_path, "/");
      full_path = str_append(&full_path, entry);

      free(entry);
    }
  }

  if (file_exists(full_path) && !is_dir(full_path)) {
    return full_path;
  }

  char* with_ext = try_resolve(full_path);

  if (with_ext && file_exists(with_ext))
    return with_ext;

  return path;
}

char* remove_whitespace(char* source)
{
  char* newstr = 0;
  size_t len = strlen(source);

  for (unsigned int i = 0; i < len; i++) {
    char c = source[i];

    if (c == '\n' || c == '\t' || c == '\r' || c == 10 || c == ' ')
      continue;

    newstr = str_append(&newstr, (char[]){ c, 0 });
  }

  return newstr;
}

char* get_indent(unsigned int size)
{
  char* str = 0;

  for (unsigned int i = 0; i < size; i++) {
    str = str_append(&str, "-");
  }

  return str ? str : strdup("");
}
