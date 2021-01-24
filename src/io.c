#include "include/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* fjb_read_file(const char* filename)
{
  FILE* fp;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("Could not read file `%s`\n", filename);
    return 0;
  }

  char* buffer = (char*)calloc(1, sizeof(char));
  buffer[0] = '\0';

  while ((read = getline(&line, &len, fp)) != -1) {
    buffer = (char*)realloc(buffer, (strlen(buffer) + strlen(line) + 1) * sizeof(char));
    strcat(buffer, line);
  }

  fclose(fp);
  if (line)
    free(line);

  return buffer;
}

void fjb_write_file(const char* filename, char* outbuffer)
{
  FILE* fp;

  fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("Could not open file for writing `%s`\n", filename);
    exit(1);
  }

  fputs(outbuffer, fp);

  fclose(fp);
}

char* sh(const char* cmd)
{
  char* output = (char*)calloc(1, sizeof(char));
  output[0] = '\0';

  FILE* fp;
  char path[1035];

  fp = popen(cmd, "r");

  if (fp == NULL) {
    printf("Failed to run command\n");
    exit(1);
  }

  while (fgets(path, sizeof(path), fp) != NULL) {
    output = (char*)realloc(output, (strlen(output) + strlen(path) + 1) * sizeof(char));
    strcat(output, path);
  }

  pclose(fp);

  return output;
}

unsigned int file_exists(char* filepath)
{
  return access(filepath, F_OK) == 0;
}
