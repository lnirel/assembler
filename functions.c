
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**

* Changes the extension of the given input file name to the specified output extension.
* @param input The original file name whose extension is to be changed.
* @param output_extension The new extension to be set for the file.
* @param output The buffer to store the resulting file name with the new extension.
*/
void change_file_extension(char* input, char* output_extension, char* output) {
    char* last_dot = strrchr(input, '.');
    if (last_dot == NULL) {
        strcpy(output, input);
    } else {
        size_t basename_len = last_dot - input;
        strncpy(output, input, basename_len);
        output[basename_len] = '\0';
    }
    strcat(output, ".");
    strcat(output, output_extension);
}

/**
 Copy contents of the input file to the output file.
* @param input_filename The name of the input file to be copied.
* @param output_filename The name of the output file to which the input file will be copied.
* @return void
*/
void copy_file(char* input_filename, char* output_filename)  {
    int c;
    FILE* output_file;
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Error: Failed to open input file\n");
        exit(1);
    }

    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Error: Failed to create output file\n");
        exit(1);
    }

    while ((c = fgetc(input_file)) != EOF) {
        fputc(c, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}


/**
* Replaces all occurrences of '0' with '.' and '1' with '/' in the given file.
* @param filename A string representing the path to the file to be modified.
* @return void
*/
void replace_chars( char *filename) {
    char c;
    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) {
        printf("Failed to open file %s\n", filename);
        return;
    }
    while ((c = fgetc(fp)) != EOF) {
        if (c == '0') {
            fseek(fp, -1L, SEEK_CUR);
            fputc('.', fp);
        } else if (c == '1') {
            fseek(fp, -1L, SEEK_CUR);
            fputc('/', fp);
        }
    }
    fclose(fp);

}

/**
* Adds IC_DC array values to the beginning of a text file and increments line numbers by 100.
* @param filename The name of the file to modify.
* @param IC_DC An array containing two integers, the IC and DC values to add to the beginning of the file.
* @return void
*/
void addCharToFile(const char* filename,int IC_DC[]){
    FILE *outputFile;
    int counter=100;
    char str[100]="haha ";
    FILE *inputFile = fopen(filename, "r");
    FILE *tempFile = tmpfile();
    char line[1024];

    if (inputFile == NULL) {
        printf("Error opening input file.\n");
        return;
    }
    if (tempFile == NULL) {
        printf("Error creating temporary file.\n");
        return;
    }
    fprintf(tempFile, "\t%d %d \n", IC_DC[0], IC_DC[1]);

    while (fgets(line, sizeof(line), inputFile)) {
        sprintf(str, "%04d\t", counter);
        fputs(str, tempFile);
        fputs(line, tempFile);
        counter++;
    }

    fclose(inputFile);
    rewind(tempFile);

     outputFile = fopen(filename, "w");
    if (outputFile == NULL) {
        printf("Error opening output file.\n");
        return;
    }

    while (fgets(line, sizeof(line), tempFile)) {
        fputs(line, outputFile);
    }

    fclose(tempFile);
    fclose(outputFile);

    remove(tempFile);
}

/**
* Checks whether the given file path has a ".as" extension.
* @param path A null-terminated string representing the file path.
* @return Returns 1 if the file path has a ".as" extension, or 0 otherwise.
*/
int has_as_extension(const char* path) {
    size_t len = strlen(path);
    const char* ext = ".as";
    size_t ext_len = strlen(ext);

    if (len < ext_len) {
        return 0;
    }

    return (strcmp(path + len - ext_len, ext) == 0);
}
