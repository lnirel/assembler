
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void change_file_extension(char* input, char* output_extension, char* output);
void copy_file(char* input_filename, char* output_filename);
void replace_chars(char *filename);
void addCharToFile(const char* filename,int IC_DC[]);
int has_as_extension(const char* path);