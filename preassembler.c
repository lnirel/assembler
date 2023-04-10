/**
* @file preassembler.c
* a program that performs preprocessing on an input file before it is assembled into machine code.
* The preassembler performs the following tasks:
* Reads the input file and extracts any macros defined in the file.
* Stores the macros in a linked list.
* Deletes the macros from the input file.
* Expands any macro calls in the input file using the stored macros.
* Writes the preprocessed input file to a new file.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_SIZE 81
#define MAX_WORD_SIZE 81

/**
* Macro data structure used to represent a macro.
* A macro consists of a name and a value.
*/
struct Macro {
    char name[MAX_WORD_SIZE];
    char value[MAX_WORD_SIZE];
    struct Macro *next;
};


/**
* Reads a file and adds macros defined in it to a linked list.
* This function reads the input file and extracts any macros defined in the file. It then stores the macros in a
* linked list.
* @param filename The name of the input file to read.
* @param head_ptr A pointer to the head of the macro linked list.
*/
void add_macros_to_list(char *filename, struct Macro **head_ptr) {
    FILE *file;
    char line[MAX_LINE_SIZE];
    char *word;
    int in_macro = 0;
    struct Macro *tail = NULL;

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
        if (strstr(line, "endmcr") != NULL) {
            in_macro = 0;
        } else if (in_macro) {
            word = strtok(line, " \n");
            while (word != NULL) {
                if (tail != NULL) {
                    strcat(tail->value, word);
                    strcat(tail->value, " ");
                }
                word = strtok(NULL, " \n");
            }
            strcat(tail->value, "\n");
        } else if (strstr(line, "mcr") != NULL) {
            in_macro = 1;
            struct Macro *macro = malloc(sizeof(struct Macro));
            if (macro == NULL) {
                printf("Error: Unable to allocate memory\n");
                return;
            }
            word = strtok(line, " \n");
            word = strtok(NULL, " \n");
            strcpy(macro->name, word);
            macro->value[0] = '\0';
            macro->next = NULL;
            if (*head_ptr == NULL) {
                *head_ptr = tail = macro;
            } else {
                tail = tail->next = macro;
            }
        }
    }

    fclose(file);
}

/**
 * Checks if the given macro key is present in the linked list of macros.
 *
 * @param head A pointer to the head of the linked list to search.
 * @param key The macro name to search for.
 * @return 1 if the macro is found, 0 otherwise.
 */int is_key(struct Macro *head, const char *key) {
    struct Macro *curr = head;
    while (curr != NULL) {
        if (strcmp(curr->name, key) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

/**
* Returns the value of the macro with the given name.
* @param name The name of the macro to look for.
* @param head A pointer to the head of the macro linked list.
* @return The value of the macro if found, otherwise NULL.
*/
char* get_value( const char *name,struct Macro *head) {
    struct Macro *curr = head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            /* Found a macro with a matching name, return its value*/
            return curr->value;
        }
        curr = curr->next;
    }
    /* No macro with a matching name was found, return NULL*/
    return NULL;
}

/**
 * Reads a file, looks for macros in it,
* replaces the macros with their corresponding values and writes the modified contents to a temporary file,
* which is then moved to replace the original file.
 * @param head A pointer to the head of the macro linked list.
 * @param file_path The path of the file to process.
*/
void macro_thread(struct Macro* head, char* file_path) {
    /* Open the file for reading*/
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", file_path);
        return;
    }

    /* Create a temporary file to write the modified contents to*/
    char temp_file_path[] = "temp.txt";
    FILE* temp_fp = fopen(temp_file_path, "w");
    if (temp_fp == NULL) {
        printf("Error creating temporary file\n");
        fclose(fp);
        return;
    }

    /* Read the file line by line*/
    char line[100];
    while (fgets(line, 100, fp) != NULL) {
        /* Split the line into words*/
        char* word = strtok(line, " \t\n");

        while (word != NULL) {
            /*Check if the word is a key*/
            if (is_key(head, word)) {
                /*Replace the word with the macro value*/
                char* macro_value = get_value(word, head);
                int macro_len = strlen(macro_value);
                /* Remove the newline character at the end of the macro value*/
                if (macro_value[macro_len - 1] == '\n') {
                    macro_value[macro_len - 1] = '\0';
                }
                fputs(macro_value, temp_fp);
            } else {
                /* Write the original word to the temporary file*/
                fputs(word, temp_fp);
            }

            /*Write a space after the word*/
            fputs(" ", temp_fp);

            /* Get the next word*/
            word = strtok(NULL, " \t\n");
        }

        /*Write a line break at the end of the line*/
        fputs("\n", temp_fp);
    }

    /* Close the input file and the temporary file*/
    fclose(fp);
    fclose(temp_fp);

    /* Replace the original file with the temporary file*/
    remove(file_path);
    rename(temp_file_path, file_path);
}

/**
 * Reads a file, deletes all the macro definitions and writes the modified contents to a temporary file,
* which is then moved to replace the original file.
 * @param filename The path of the file to process.
*/
void delete_macros(char *filename) {
    FILE *fp, *tmpfp;
    char line[1024];
    int in_macro = 0;
    int prev_was_mcr = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: cannot open file %s\n", filename);
        exit(1);
    }

    tmpfp = tmpfile();  /* open a temporary file for writing*/
    if (tmpfp == NULL) {
        printf("Error: cannot create temporary file\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "mcr", 3) == 0) {
            in_macro = 1;
            prev_was_mcr = 1;
            continue;  /* skip the "mcr" line*/
        } else if (strncmp(line, "endmcr", 6) == 0) {
            in_macro = 0;
            prev_was_mcr = 0;
            continue;  /* skip the "endmcr" line*/
        } else if (in_macro == 1) {
            prev_was_mcr = 0;
            continue;  /*skip the line*/
        }

        if (prev_was_mcr == 1) {
            prev_was_mcr = 0;
            continue;  /* skip the line*/
        }

        fputs(line, tmpfp);  /* write the line to the temporary file*/
    }

    fclose(fp);
    rewind(tmpfp);

    fp = fopen(filename, "w");  /*open the original file for writing*/
    if (fp == NULL) {
        printf("Error: cannot open file %s for writing\n", filename);
        exit(1);
    }

    while (fgets(line, sizeof(line), tmpfp) != NULL) {
        fputs(line, fp);  /* write the line to the original file*/
    }

    fclose(fp);
    fclose(tmpfp);
    remove(tmpfp);
}



void preassembler(char *input_file) {
    struct Macro *head = NULL;

    add_macros_to_list(input_file, &head);
    delete_macros(input_file);
    /* Print macros*/
    struct Macro *curr = head;
    while (curr != NULL) {
        curr = curr->next;
    }

    macro_thread(head, input_file);

    /* Free memory*/
    curr = head;
    while (curr != NULL) {
        struct Macro *temp = curr;
        curr = curr->next;
        free(temp);
    }

}
