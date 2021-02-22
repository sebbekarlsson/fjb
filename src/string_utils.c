#include "include/string_utils.h"
#include "include/constants.h"
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

  size_t slen = src ? strlen(src) : 0;
  size_t plen = piece ? strlen(piece) : 0;
  size_t len = slen + plen + 1;
  src = realloc(src, len * sizeof(char));
  memcpy(src + slen, piece, plen + 1);

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

void get_dirs(char* path, char*** dirs, unsigned int* len)
{
  if (!path)
    return;

  char** _dirs = 0;
  char* _path = strdup(path);
  char* tok = strtok(_path, "/");
  unsigned int _len = 0;

  while (tok != 0) {
    _len += 1;

    if (!_dirs) {
      _dirs = calloc(_len, sizeof(char*));
    } else {
      _dirs = realloc(_dirs, sizeof(char*) * _len);
    }

    _dirs[_len - 1] = strdup(tok);

    tok = strtok(0, "/");
  }

  *dirs = _dirs;
  *len = _len;
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
  int v = (int)x;
  if (v == x)
    return int_to_str(v);

  char* str = calloc(32, sizeof(char));
  sprintf(str, "%f", x);

  for (unsigned int i = strlen(str); i > 0; i--) {
    if (str[i - 1] == '0')
      str[i - 1] = 0;
    else
      break;
  }

  return str;
}

unsigned int is_special(char c)
{
  const char* specials = "\"!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
  return strchr(specials, c) != (void*)0;
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
  if (!filepath)
    return 0;

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

char* get_filename(char* path)
{
  char* _path = strdup(path);
  char* tok = strtok(_path, "/");
  if (!tok)
    return 0;

  char** parts = calloc(1, sizeof(char*));
  unsigned int len = 0;

  unsigned int i = 0;
  while (tok != 0) {
    if (!parts) {
      len += 1;
      parts = calloc(len, sizeof(char*) + (strlen(tok) * sizeof(char)));
      i += 1;
    } else {
      len += 1;
      parts = realloc(parts, len * (sizeof(char*) + (strlen(tok) * sizeof(char))));
      i += 1;
    }
    parts[len - 1] = strdup(tok);
    tok = strtok(0, "/");
  }

  if (!len || !parts)
    return path;

  unsigned int index = MAX(0, len - 1);
  char* last = len >= index ? parts[index] : 0;

  if (parts)
    free(parts);

  if (_path)
    free(_path);

  return last;
}

char* get_slashed_path(char* path)
{
  if (!path)
    return 0;

  char* _path = strdup(path);
  char* tok = strtok(_path, "/");

  if (!tok)
    return 0;

  char** parts = 0;
  unsigned int len = 0;

  while (tok != 0) {
    if (!parts) {
      len += 1;
      parts = calloc(len, sizeof(char*));
    } else {
      len += 1;
      parts = realloc(parts, len * sizeof(char*));
    }
    parts[len - 1] = strdup(tok);
    tok = strtok(0, "/");
  }

  if (!len || !parts)
    return path;

  unsigned int index = MAX(0, len - 2);
  char* last = parts[index];
  char* fname = get_filename(path);
  if (!fname)
    return 0;

  char* str = 0;
  str = str_append(&str, last);
  str = str_append(&str, "/");
  str = str_append(&str, fname);

  if (parts)
    free(parts);

  if (_path)
    free(_path);

  return str;
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

const char* extensions[] = { ".js", ".ts", ".jsx", ".tsx" };
const size_t nr_extensions = 4;

char* try_resolve_index(char* path)
{
  if (is_dir(path)) {
    char* entry = get_entry(path);
    if (entry) {
      char* path_to_entry = 0;
      path_to_entry = str_append(&path_to_entry, path);
      path_to_entry = str_append(&path_to_entry, "/");
      path_to_entry = str_append(&path_to_entry, entry);

      if (file_exists(path_to_entry) && !is_dir(path_to_entry)) {
        return path_to_entry;
      }
    }
  }
  for (unsigned int i = 0; i < nr_extensions; i++) {
    char buff[360];
    sprintf(buff, "%s/index%s", path, extensions[i]);
    if (file_exists(buff) && !is_dir(buff))
      return strdup(buff);
  }

  return 0;
}

char* try_resolve(char* path)
{
  for (unsigned int i = 0; i < nr_extensions; i++) {
    char buff[360];
    sprintf(buff, "%s%s", path, extensions[i]);

    if (file_exists(buff) && !is_dir(buff))
      return strdup(buff);
  }

  return 0;
}

char* resolve_file(char* basepath, char* filepath)
{

  char* full_path = 0;
  char* with_e = try_resolve(filepath);

  if (with_e)
    return with_e;

  char* path_to_file = 0;
  path_to_file = str_append(&path_to_file, basepath);
  path_to_file = str_append(&path_to_file, "/");
  path_to_file = str_append(&path_to_file, filepath);
  char* index = try_resolve_index(path_to_file);

  char* m = try_resolve(path_to_file);
  if (m)
    return m;

  if (index)
    return index;

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

  with_e = try_resolve(full_path);

  if (with_e)
    return with_e;

  return 0;
}

char* try_every_dir(char* basepath, char* filepath)
{
  if (!basepath)
    return 0;
  char** dirs = 0;
  unsigned int len = 0;
  get_dirs(basepath, &dirs, &len);

  char* full = 0;

  for (unsigned int i = 0; i < len; i++) {
    char* dir = dirs[i];
    if (!dir)
      continue;

    full = str_append(&full, dir);
    full = str_append(&full, "/");
    char* to_check = 0;
    to_check = str_append(&to_check, full);
    to_check = str_append(&to_check, "node_modules");

    char* maybe = resolve_file(to_check, filepath);
    if (maybe)
      return maybe;
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

  if (full_path && is_dir(full_path)) {
    char* entry = get_entry(full_path);

    if (entry) {
      full_path = str_append(&full_path, "/");
      full_path = str_append(&full_path, entry);

      free(entry);
    }
  }

  if (full_path && file_exists(full_path) && !is_dir(full_path)) {
    return full_path;
  }

  char* with_ext = try_resolve(full_path);

  if (with_ext && file_exists(with_ext))
    return with_ext;

  if (!path) {
    char* check_path = find_in_path(basepath, filepath);

    if (check_path && file_exists(check_path) && !is_dir(check_path))
      return check_path;

    if (check_path) {
      char* v = resolve_file(check_path, filepath);
      if (v)
        return v;
    }
  }

  if (!path) {
    path = try_every_dir(basepath, filepath);
  }

  if (file_exists(path) && !is_dir(path))
    return path;

  printf("Unable to resolve `%s`\n", filepath);
  exit(1);
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

char char_tolower(unsigned char ch)
{
  if (ch >= 'A' && ch <= 'Z')
    ch = 'a' + (ch - 'A');
  return ch;
}

char* strlow(char* source)
{
  unsigned int len = strlen(source);

  for (unsigned int i = 0; i < len; i++)
    source[i] = char_tolower(source[i]);

  return source;
}

int strcasecmp(const char* s1, const char* s2)
{
  const unsigned char *us1 = (const u_char*)s1, *us2 = (const u_char*)s2;

  while (char_tolower(*us1) == char_tolower(*us2++))
    if (*us1++ == '\0')
      return (0);
  return (char_tolower(*us1) - char_tolower(*--us2));
}

unsigned int str_contains(char* haystack, char* needle)
{
  return (strstr(haystack, needle) != 0);
}

char* find_in_path(char* path, char* filename)
{
  if (!path)
    return 0;

  char* _path = strdup(path);
  char* token = strtok(_path, "/");
  char* new_path = 0;

  if (_path[0] == '/')
    new_path = str_append(&new_path, "/");

  while (token != 0) {
    new_path = str_append(&new_path, token);

    char* check_path = 0;
    check_path = str_append(&check_path, new_path);
    check_path = str_append(&check_path, "/");
    check_path = str_append(&check_path, filename);

    if (file_exists(check_path) || is_dir(check_path))
      return new_path;

    char* maybe = try_resolve(check_path);
    if (!maybe)
      maybe = try_resolve_index(check_path);

    if (maybe)
      return maybe;

    free(check_path);
    token = strtok(0, "/");
    new_path = str_append(&new_path, "/");
  }

  return 0;
}

char* str_get_after(char* source, char* after)
{
  if (!str_contains(source, after))
    return source;

  char* found = strstr(source, after);
  int pos = (found - source) + strlen(after);
  char* v = source + pos;
  if (!v)
    return source;
  return source + pos;
}
