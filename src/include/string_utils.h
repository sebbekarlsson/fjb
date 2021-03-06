#ifndef FJB_STRING_UTILS_H
#define FJB_STRING_UTILS_H

char* str_append(char** source, const char* piece);

char* str_prefix(char** source, const char* piece);

unsigned int str_contains(char* haystack, char* needle);

char* str_encode(char* source);

char* charstr(char c);

char* dirname(const char* path);

const char* get_filename_ext(char* filename);

char* int_to_str(int x);

char* float_to_str(float x);

unsigned int is_special(char c);

char* resolve_import(char* basepath, char* filepath, unsigned int node_modules);

char* remove_whitespace(char* source);

char* strlow(char* source);

char* get_indent(unsigned int size);

char* str_get_after(char* source, char* after);

char* find_in_path(char* path, char* filename);

char* get_basename(char* filepath);

char* get_slashed_path(char* path);

char char_tolower(unsigned char ch);

char* strip_ext(char* filepath);

int strcasecmp(const char* s1, const char* s2);
#endif
