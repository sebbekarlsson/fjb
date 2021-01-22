#ifndef FJB_STRING_UTILS_H
#define FJB_STRING_UTILS_H

char* str_append(char** source, const char* piece);

char* str_prefix(char* source, const char* piece);

char* str_encode(char* source);

char* charstr(char c);

char* dirname(const char* path);

const char* extension(const char* path);

char* remove_char(char* str, char find);

const char* get_filename_ext(const char* filename);

char* int_to_str(int x);

char* float_to_str(float x);

unsigned int is_special(char c);

unsigned int first_char_is_special(char* str);

char* resolve_import(char* basepath, char* filepath);

char* remove_whitespace(char* source);
#endif
