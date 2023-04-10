#include <stdio.h>
#include "ht.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LABEL_LENGTH 81
#define MAX_LINE_SIZE 81
static int counter=0;
static int errors=0;

/**
 * This function takes a string as input and returns the number of letters inside quotation marks
 * (the number of characters between two consecutive quotation marks excluding the whitespace characters).
 * @param str the input string
 * @return the number of letters inside quotation marks
 */
int count_letters_in_quotes(char* str) {
    int len = strlen(str);
    int count = 0;
    int insideQuotes = 0;
    int i;
    for ( i = 0; i < len; i++) {
        if (str[i] == '"') {
            insideQuotes = !insideQuotes;
        } else if (insideQuotes && str[i] != ' ') {
            count++;
        }
    }
    return count;
}

/**
 * This function takes a string as input and returns the number of numbers in the string.
 * A number can start with an optional '+' or '-' sign, followed by one or more digits.
 * Numbers are separated by commas and whitespace characters.
 * @param str the input string
 * @return the number of numbers in the string
 */
int count_numbers( char* str) {
    int count = 0;
    int inside_quotes = 0;
    int len = strlen(str);
    int i;
    for ( i = 0; i < len; i++) {
        if (isspace(str[i])) {
            inside_quotes = 0;
        } else if (isdigit(str[i]) || str[i] == '-' || str[i] == '+') {
            /* Found the start of a number*/
            if (!inside_quotes) {
                count++;
                inside_quotes = 1;
            }
        } else if (str[i] == ',') {
            /*Found a comma, reset the inNumber flag*/
            inside_quotes = 0;
        }
    }
    return count;
}

/**
 * This function removes the label (if any) from the input string and returns 1,
 * adding the label to the hash table with value of IC if a label was found and removed, 0 otherwise.
 * @param input the input string
 * @param IC the instruction counter
 * @param ht the hash table
 * @return 1 if a label was found and removed, 0 otherwise
 */
int remove_label(char* input, int IC,HashTable* ht) {
    int i = 0;
    if (!isalpha(input[i])) { /* if first character is not a letter, it's not a label */
        return 0;
    }
    i++;
    while (isalnum(input[i]) && i < MAX_LABEL_LENGTH) {
        i++;
    }
    if (input[i] == ':') {
        char subStr[MAX_LINE_SIZE];
        strncpy(subStr,input,i);
        if(searchWithSpaces(ht,subStr)!=-1){
            printf("Error: the label %s is already defined, line: %d\n",subStr,counter);
            errors=-1;
        }
        insert(ht,subStr,IC+100);
        memmove(input, input+i+1, strlen(input+i)); /* remove label and ':' from input*/
        return 1;
    }
    return 0;
}

/**
 * This function takes a string as input and returns 1 if the string represents a valid register
 * (i.e., a string of length 2 starting with 'r' followed by a digit between 0 and 7), 0 otherwise.
 * @param str The input string to check.
 * @return 1 if the string represents a valid register (i.e., a string of length 2 starting with 'r' followed by a digit between 0 and 7), 0 otherwise.
*/
int is_register( char* str) {
    return (strlen(str) == 2 && str[0] == 'r' && isdigit(str[1]) && str[1] >= '0' && str[1] <= '7');
}

/**
 * This function takes a string as input and returns 1 if the string represents a valid number
 * (i.e., a string starting with '#' followed by an optional '+' or '-' sign and one or more digits), 0 otherwise.
 * @param str The input string to check.
 * @return 1 if the string represents a valid number, 0 otherwise.
 */
int is_number( char* str) {
    int i = 0;
    if (str[i] != '#') {
        return 0;
    }
    i++;
    if (str[i] == '+' || str[i] == '-') {
        i++;
    }
    while (isdigit(str[i])) {
        i++;
    }
    return (i > 0 && str[i] == '\0');
}

/**
 * This function takes a string as input and returns 1 if the string represents a valid label
 * (i.e., a string starting with a letter followed by zero or more alphanumeric or underscore characters and ending with the end of the string), 0 otherwise.
 * @param str The input string to check.
 * @return 1 if the string represents a valid label, 0 otherwise.
 * */
int is_label( char* str) {
    int i = 0;
    if (!isalpha(str[i])) {  /* check if the first character is a letter*/
        return 0;
    }
    i++;
    while (isalnum(str[i]) || str[i] == '_') {  /* check if the rest of the string contains only letters, digits, and underscores*/
        i++;
    }
    return (i > 0 && str[i] == '\0');  /*check if there are no additional characters*/
}



/**
 * This function checks the second word in the input string.
 *If the second word is a register, it returns 3.
 * If the second word is a number, it returns 0.
 * If the second word is a label, it returns 1.
 * Otherwise, it returns -1.
 * @param str The input string to check.
 * @return 3 if the second word is a register, 0 if the second word is a number, 1 if the second word is a label, -1 otherwise.
 */
int check_second_word(char* str) {
    char first_word[80+1];
    int i = 0;
    char* word_ptr;
    char* comma_ptr = strchr(str, ',');  /*Find the first comma in the input string*/
    if (comma_ptr == NULL) {  /* If no comma found, print an error */
        printf("Error: No comma found in input string in line %d\n",counter);
        errors=-1;
        return -1;
    }
    word_ptr = comma_ptr + 1;  /*Point to the character after the comma*/

    while (*word_ptr == ' ') {  /* Skip any leading whitespace characters*/
        word_ptr++;
    }
    while (str[i] && !isspace(word_ptr[i]) && i < 81) {
        first_word[i] = word_ptr[i];
        i++;
    }
    first_word[i] = '\0';

    if (is_register(first_word)) {
        return 3;
    } else if (is_number(first_word)) {
        return 0;
    } else if (is_label(first_word)) {
        return 1;
    } else {
        printf("Unknown Second word: %s in line %d\n", first_word,counter);
        errors=-1;

    }
    return -1;
}

/**
 * This function checks the first word in the input string.
 * If the first word is a register, it returns 3.
 * If the first word is a number, it returns 0.
 * If the first word is a valid label, it returns 1.
 * Otherwise, it returns -1.
 *  @param str: The input string to check.
 * @return 3 if the second word is a register, 0 if the second word is a number, 1 if the second word is a label, -1 otherwise.
 * */
int check_first_word(char* str) {
    char first_word[MAX_LABEL_LENGTH+1];
    int i = 0;
    while (str[i] && !isspace(str[i]) && str[i] != ',' && i < MAX_LABEL_LENGTH) {
        first_word[i] = str[i];
        i++;
    }
    first_word[i] = '\0';
    if (is_register(first_word)) {
        return 3;
    } else if (is_number(first_word)) {
        return 0;
    } else if (is_label(first_word)) {
        return 1;
    } else {
        printf("Unknown first word: %s in line %d\n", first_word,counter);
        errors=-1;

    }
    return -1;
}


/**
 * Removes all characters from the beginning of the given string up to and including the first open parenthesis,
 * and up to but not including the first closing parenthesis that comes after the open parenthesis.
 * If the parentheses are not found or if the closing parenthesis comes before the opening parenthesis, no characters are removed.
 * @param str: The input string to modify.
 * @return If the parentheses are found and the closing one is after the opening one, it returns 1. Otherwise, it returns 0.
 */
int remove_before_open_parenthesis(char* str) {
    char* open_paren = strchr(str, '('); /*find the first occurrence of an open parenthesis*/
    char* close_paren = strchr(str, ')'); /* find the first occurrence of a closing parenthesis*/
    if (open_paren != NULL && close_paren != NULL && close_paren > open_paren) { /*if we found both parentheses and the closing one is after the opening one*/
        memmove(str, open_paren+1, close_paren-open_paren); /*use memmove() to shift the string to the beginning*/
        str[close_paren - open_paren - 1] = '\0'; /*set the new end of the string to the character before the closing parenthesis*/
        return 1;
    } else { /*if there are no parentheses or the closing one is before the opening one*/
        return 0;
    }
}

/**
 * Determines if a string has multiple words.
 *
 * This function takes a pointer to a null-terminated string and determines if it contains
 * more than one word. A word is defined as a sequence of non-space characters. The function
 * returns 1 if the string has more than one word and 0 otherwise.
 *
 * @param str Pointer to a null-terminated string.
 * @return 1 if the string has more than one word, 0 otherwise.
 */
int hasMultipleWords(const char *str) {
    // Create a copy of the string to avoid modifying the original
    char *copy = strdup(str);

    // Initialize a counter for the number of words

int hasMultipleWords(const char *str) {
    char *copy = strdup(str);
    int count = 0;
    char *token = strtok(copy, " ,");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " ,");
    }
    free(copy);
    return (count > 1);
}

/**
 * Finds the number of words needed for the given instruction and input_remainder.
 * @param instruction: The instruction to perform.
 * @param input_remainder: The remaining input string to check.
 * @param ht: The symbol table hash table.
 * @param entryht: The entries table hash table.
 * @return The number of words needed for the instruction.
 */
int findL( char* instruction,  char* input_remainder, HashTable* ht,HashTable* enteryht,int *DC, int *IC) {

    /* Compare the instruction with the known keywords*/
    int tmp_num,errors=0;
    int type1=-1,type2=-1;
    int L=1;
    if (!strcmp(instruction, "mov") || !strcmp(instruction, "cmp") || !(strcmp(instruction, "add")) || !(strcmp(instruction, "sub")) || !(strcmp(instruction, "lea"))) {

        type1 = check_first_word(input_remainder);
        type2 = check_second_word(input_remainder);
        if((type1==3 || type1==0) && !strcmp(instruction, "lea")) {
            printf("Error: Invalid address method for the source operand, line: %d\n", counter);
            errors=-1;
        }
        if(type2==0 && strcmp(instruction, "cmp")) {
            printf("Error: Invalid address method for the destination operand, line: %d\n", counter);
            errors=-1;
        }
        if(type1==type2 && type1==3){
            L++;
        }
        else{
            L+=2;
        }

    } else if (!strcmp(instruction, "not") || !(strcmp(instruction, "clr")) || !(strcmp(instruction, "inc"))
               || !(strcmp(instruction, "dec")) || !(strcmp(instruction, "red"))) {
        type1= check_first_word(input_remainder);
        if(type1 ==0){
            printf("Error: Invalid address method for the destination operand, line: %d\n", counter);
            errors=-1;

        }
        if(hasMultipleWords(input_remainder)){
            printf("Error: more opreands then allowd in %s %s, line: %d\n",instruction,input_remainder,counter);
            errors=-1;

        }
        L++;
    }
    else if (!strcmp(instruction, "jmp") || !strcmp(instruction, "bne") || !strcmp(instruction, "jsr")) {
        if(remove_before_open_parenthesis(input_remainder)){
            L++;
            type1= check_first_word(input_remainder);
            type2= check_second_word(input_remainder);
            if(type1==type2 && type1==3){
                L++;
            } else{
                L+=2;
            }
        } else{
            type1= check_first_word(input_remainder);
            if(type1==0){
                printf("Error: Invalid address method for the destination operand, line: %d\n", counter);
                errors=-1;
            }
            L++;
        }

    }
    else if (strcmp(instruction, "prn") == 0) {
        type1 = check_first_word(input_remainder);
        L++;
    } else if (!strcmp(instruction, "rts") || !strcmp(instruction, "stop")) {


    }
    else if(!strcmp(instruction, ".data")){
        tmp_num= count_numbers(input_remainder)-1;
        L+=tmp_num;
        *DC+=tmp_num+1;

    }
    else if(!strcmp(instruction,".string")){
        tmp_num=count_letters_in_quotes(input_remainder);
        L+= tmp_num;
        *DC+=tmp_num+1;



    }
    else if(!strcmp(instruction,".extern")){
        insert(ht,input_remainder,-2);
        L=0;
    }
    else if(!strcmp(instruction,".entry")){
        L=0;
        insert(enteryht,input_remainder,1);

    }
    else if(instruction[0]==';'){
        L=0;
    }
    else {
        printf("Unknown instruction: %s in line %d\n", instruction,counter);
        errors=-1;

        L=0;
        errors=-1;
    }
    *IC+=L;

    return errors;
}


/**
* Executes the first stage of the assembler program.
* Reads the input file line by line, removes any labels from the line,
* and calls the findL function to process the instruction and its operands.
* @param input_file Pointer to the input file to be processed.
* @param ht Pointer to the HashTable object that stores the symbols table.
* @param enteryht Pointer to the HashTable object that stores the entries table.
* @return Void.
*/
int mainfirstStage(char* input_filename, HashTable* ht, HashTable* entryht,int IC_DC[]) {

    int IC=0;
    int DC=0;
    FILE* file = fopen(input_filename, "r");
    char str[MAX_LINE_SIZE];

    while (fscanf(file, "%[^\n]\n", str) == 1) {
        char* instruction;
        char* input_remainder;
        remove_label(str,IC,ht);
        instruction = strtok(str, " "); /* Extract the first word as the instruction*/
        input_remainder = strtok(NULL, ""); /*Extract the remainder of the input*/
        errors+=findL(instruction, input_remainder,ht,entryht,&DC,&IC);
        counter++;
        counter++;

    }

    IC_DC[0]=IC-DC;
    IC_DC[1]=DC;
    fclose(file);

    return errors;
}
