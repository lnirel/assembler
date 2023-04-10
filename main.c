#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "preassembler.h"
#include "first_pass.h"
#include "second_pass.h"
int main(int argc, char *argv[]) {
    int i,errors;
    int IC_DC[2]={0,0};

    for (i = 1; i < argc; i++) {
        char *input_filename = argv[i];
        if(!has_as_extension(input_filename))/*if its not an as file*/
            continue;

            HashTable ht;
        HashTable enteryht;

        /* Allocate memory for the output file path*/
        char *macro_filename = (char *) malloc(strlen(input_filename) + strlen("am") + 2);
        char *object_filename = (char *) malloc(strlen(input_filename) + strlen("ob") + 2);
        char *extern_filename = (char *) malloc(strlen(input_filename) + strlen("ext") + 3);
        char *entry_filename = (char *) malloc(strlen(input_filename) + strlen("ent") + 3);


        /* Call the function to change the file extension*/
        change_file_extension(input_filename, "am", macro_filename);
        change_file_extension(input_filename, "ob", object_filename);
        change_file_extension(input_filename, "ext", extern_filename);
        change_file_extension(input_filename, "ent", entry_filename);

        copy_file(input_filename, macro_filename);
        preassembler(macro_filename);

        initHashTable(&ht);
        initHashTable(&enteryht);


        errors=mainfirstStage(macro_filename, &ht, &enteryht,IC_DC);/*the first pass*/
        secondStage(&ht, &enteryht, macro_filename, object_filename, extern_filename, entry_filename);/*thesecond pass*/

        replace_chars(object_filename);/*replces the 0 and 1 to . and / */
        addCharToFile(object_filename,IC_DC);


        if(errors==-1){/*removes the files if theres an error*/
            remove(macro_filename);
            remove(object_filename);
            remove(extern_filename);
            remove(entry_filename);
        }
        /* Free the memory allocated for the output file path*/

        free(macro_filename);
        free(object_filename);
        free(extern_filename);
        free(entry_filename);
    }
    return 0;
}
