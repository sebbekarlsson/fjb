#ifndef FJB_IO_H
#define FJB_IO_H

char* fjb_read_file(const char* filename);
void fjb_write_file(const char* filename, char* outbuffer);
char* sh(const char* cmd);
unsigned int file_exists(char* filepath);

#endif
