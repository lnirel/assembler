#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "ht.h"

#define MAX_LABEL_LENGTH 81
#define MAX_LINE_SIZE 81

int remove_labell(char* input, int IC,HashTable* entryht,char* entryfilepath) {
    FILE *entryfp;/*entry file path*/
    int i = 0;
    if (!isalpha(input[i])) { /* if first character is not a letter, it's not a label*/
        return 0;
    }

    i++;
    while (isalnum(input[i]) && i < MAX_LINE_SIZE) {
        i++;
    }
    if (input[i] == ':') {
        char subStr[MAX_LINE_SIZE];
        strncpy(subStr,input,i);
        if(searchWithSpaces(entryht, subStr)==1){
            entryfp = fopen(entryfilepath, "a");
            fprintf(entryfp,"%s\t%d\n",subStr,IC+100);
            fclose(entryfp);
        }

        memmove(input, input+i+1, strlen(input+i)); /*remove label and ':' from input*/
        return 1;
    }
    return 0;
}

void remove_leading_spaces(char* str) {
    char* p = str;
    while (*p != '\0' && isspace(*p)) { /* skip over leading spaces*/
        p++;
    }
    memmove(str, p, strlen(p) + 1); /* move the non-space characters to the beginning of the string*/
}

/**
 * This function takes an input string and a pointer to an opcode string as arguments, and returns the opcode number as an integer.
 * It checks the first word of the input string against a list of valid opcodes, and if a match is found,
 * converts the opcode number to a binary string and saves it to the output parameter.
 * If no match is found, an error message is printed.
 * @param input a char pointer to the input string.
 * @param opcode a char pointer to the opcode string output.
 * @return The opcode number as an integer, or -1 if the input string is empty or an invalid opcode is found.
 * */
int get_opcode( char* input, char* opcode) {
    /* Define the list of valid opcodes*/
    char* opcodes[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int num_opcodes = sizeof(opcodes) / sizeof(opcodes[0]);
    int opcode_num,i;
    char* token;
    /* Create a copy of the input string*/
    char input_copy[81];
    strncpy(input_copy, input, sizeof(input_copy));
    input_copy[sizeof(input_copy) - 1] = '\0';

    /* Get the first word of the input string*/
    token = strtok(input_copy, " ");
    if (token == NULL) {
        printf("Error: input string is empty\n");
        return -1;
    }

    /* Find the opcode number in the list*/
     opcode_num = -1;
    for (i = 0; i < num_opcodes; i++) {
        if (strcmp(token, opcodes[i]) == 0) {
            opcode_num = i;
            break;
        }
    }
    if (opcode_num == -1 && strcmp(token,".entry") && strcmp(token,".extern") && strcmp(token,".string") && strcmp(token,".data") && token[0]!=';') {
        printf("Error: invalid opcode '%s'\n", token);
        return -1;
    }

    /* Convert the opcode number to binary with 4 digits*/

    for ( i = 3; i >= 0; i--) {
        if (opcode_num & (1 << i)) {
            opcode[3-i] = '1';
        } else {
            opcode[3-i] = '0';
        }
    }
    opcode[4] = '\0';

    return opcode_num;
}

/**
 * This function takes an input string and removes the first word (up to the first space or comma character) from it.
 * @param input a char pointer to the input string.
 * */
void remove_first_word(char* input) {
    /* Find the first space or comma character*/
    size_t remainder_len;
    char* space_ptr = strpbrk(input, " ,");
    if (space_ptr == NULL) {
        /*No space or comma found, so input is already a single word*/
        input[0] = '\0';
        return;
    }

    /* Overwrite the first word with the remainder of the string*/
     remainder_len = strlen(space_ptr+1);
    memmove(input, space_ptr+1, remainder_len+1);
}


/**
 * This function checks the first word in the input string.
 * If the first word is a register, it returns 3.
 * If the first word is a number, it returns 0.
 * If the first word is a valid label, it returns 1.
 * Otherwise, it returns -1.
 * @param input a const char pointer to the input string.
 * @param first_operand a char pointer to the first_operand string.
 * @return 3 if the second word is a register, 0 if the second word is a number, 1 if the second word is a label, -1 otherwise.
 * */
int get_first_operand_type( char* input, char* first_operand) {
    char copy[82];
    char* token;
    strncpy(copy, input, 81);
    copy[81] = '\0';

    token = strtok(copy, ", ");
    if (token == NULL) {
        printf("Error: input string is empty\n");
        return -1;
    }

    /*Save the first operand to the output parameter*/
    strncpy(first_operand, token, 81);
    first_operand[81] = '\0';

    /* Check if the token is a register*/
    if (strlen(token) == 2 && token[0] == 'r' && token[1] >= '0' && token[1] <= '7') {
        return 3;
    }

    /* Check if the token is a variable that starts with a letter*/
    if (strlen(token) > 0 && isalpha(token[0])) {
        int i = 1;
        while (isalnum(token[i])) {
            i++;
        }
        if (token[i] == '\0') {
            return 1;
        }
    }

    /* Check if the token is a number that starts with '#'*/
    if (strlen(token) > 0 && token[0] == '#') {
        /* Check if the token is a valid number*/
        int i = 1;
        int num;
        if (token[i] == '+') {
            i++;
        } else if (token[i] == '-') {
            i++;
        }
        num = 0;
        while (token[i] >= '0' && token[i] <= '9') {
            num = num * 10 + (token[i] - '0');
            i++;
        }
        if (token[i] == '\0') {
            return 4 + num;
        }
    }

    /*Token is not a valid register, variable, or number*/
    return -1;
}


/**
 * Converts an integer num to a binary string of a specified length of bits
 *
 * @param num The integer to be converted
 * @param num_bits The length of the binary string
 * @param buffer The buffer to store the binary string in
 * @param sign The sign of the number (+ or -), or NULL if positive
 */
void int_to_binary(int num, int num_bits, char* buffer, char sign) {

    int i;
    int is_negative = 0;

    /* Check if number is */
    if (num!=0 && sign == '-') {
        is_negative = 1;
        /* Convert number to positive using 2's complement method*/
        num = ~(num-1);
    }

    /* Convert number to binary string*/
    for (i = num_bits-1; i >= 0; i--) {
        buffer[i] = (num & 1) + '0';
        num >>= 1;
    }

    /*Add sign bit for negative numbers in 2's complement*/
    if (is_negative) {
        buffer[0] = '1';
    }

    /* Null-terminate the string*/
    buffer[num_bits] = '\0';
}

/**
* Removes all characters in a string before the first open parenthesis,
* and copies the word before the opening parenthesis to the output parameter.
* @param line The string to modify
* @param before_paren The output parameter to store the word before the opening parenthesis
* @return 1 if the operation was successful, 0 otherwise
*/
int remove_before_open_parentheses(char* line, char* before_paren) {
    char* open_paren = strchr(line, '('); /* find the first occurrence of an open parenthesis*/
    char* close_paren = strchr(line, ')'); /*find the first occurrence of a closing parenthesis*/
    if (open_paren != NULL && close_paren != NULL && close_paren > open_paren) { /* if we found both parentheses and the closing one is after the opening one*/
        memmove(line, open_paren+1, close_paren-open_paren);
        line[close_paren - open_paren - 1] = '\0';
        strncpy(before_paren, line, open_paren-line);
        before_paren[open_paren-line] = '\0';
        return 1;
    } else {/* if there are no parentheses or the closing one is before the opening one*/
        return 0;
    }
}


/**
* Adds to a file the ASCII codes of each character in a string, excluding spaces and double quotes.
* The ASCII codes are written to a file in binary format.
* @param str Pointer to the string to be processed.
* @param fp Pointer to the file where the binary codes are written.
*/
int print_ascii_codes(char* jaja, FILE *fp) {
    int i = 0,num_chars=0;
    char word[15];
    while (jaja[i] != '\0' && jaja[i] != ' ') {
        if (jaja[i] != '\"' && jaja[i] != '\\') {/*skip over double quotes and*/

            int_to_binary(jaja[i],14,word,'+');
            fprintf(fp,"%s\n",word);
            num_chars++;
        }
        i++;
    }
    fprintf(fp,"%s\n","00000000000000");
    return num_chars+1;/*return the number of chars + the char 000000000000*/
}


/**
* Separates numbers from a string separated by commas and adds to a file each number in binary.
* @param str The string containing the numbers separated by commas.
* @param fp The file pointer to write the binary numbers to.
*/
void separate_numbers(char *str,FILE *fp) {
    char binaryNum[15];
    char *token = strtok(str, ",");
    while (token != NULL) {
        char sign = '\0';
        char *num = token;
        if (num[0] == '+' || num[0] == '-') {
            sign = num[0];
            num++;
        }
        int_to_binary(atoi(num),14,binaryNum,sign);
        fprintf(fp,"%s\n",binaryNum);

        token = strtok(NULL, ",");
    }
}

/**
* Copies all characters from the beginning of the input string
* up to the first occurrence of '(' into the output string.
* @param input The input string to be copied from.
* @param output The output string to be copied into.
*/
void copyBeforeParen(char input[], char output[]) {
    int i;

    for (i = 0; input[i] != '\0' && input[i] != '('; i++) {
        output[i] = input[i];
    }
    output[i] = '\0';
}



/**
 * This function takes an input string, a file pointer, a HashTable pointer,
 * an integer, and a string as input, and generates the corresponding binary code
 * for the input string using the given HashTable and writes it to the file pointed
 * to by the file pointer. The integer input represents the current line number.
 * The string input represents the file path of the external file.
 *
 * @param input The input string to generate binary code for
 * @param fp A pointer to the file to write the binary code to
 * @param ht A pointer to the HashTable containing the symbol table
 * @param L The current line number
 * @param externfilepath The file path of the external file
 * @return 0 if successful, -1 if an error occurs
 */
int translate_assembly_line(char* input,FILE *fp,HashTable* ht,int L,char* externfilepath){
    char labels[13];
    char labels2[13];
    char labels3[13];
    int value = searchWithSpaces(ht, "LENGTH");
    FILE *efp;/*external file path*/

    char param1[3]="00";/*param1 in binary*/

    char param2[3]="00";/*param2 in binary*/

    char opcode[5]="0000";/*opcde in binary*/
    char addressing_source[3]="00";/*addressing_source in binary bits*/
    char addressing_target[3]="00";/*addressing_target in binary bits*/
    char are[3]="00";/*era in biary bits*/

    char rs[7]="000000";/*register source binary*/
    char rt[7]="000000";/*register target binary*/
    char number[13];
    char word[15];


    int opcodenum = get_opcode(input,opcode);
    if( (opcodenum>=0 && opcodenum<=3) || opcodenum==6){
        char first_operand[81];
        char second_operand[81];
        int second_operand_type;
        int first_operand_type;
        remove_first_word(input);
        first_operand_type = get_first_operand_type(input, first_operand);
        int_to_binary((first_operand_type>=4)?0:first_operand_type,2,addressing_source,'+');

        remove_first_word(input);

         second_operand_type = get_first_operand_type(input, second_operand);

        int_to_binary((second_operand_type>=4)?0:second_operand_type,2,addressing_target,'+');

        sprintf(word, "%s%s%s%s%s%s", param2,param1,opcode,addressing_source,addressing_target,are);
        fprintf(fp,"%s\n",word);
        L++;

        if(first_operand_type==second_operand_type && first_operand_type==3){
            int_to_binary(first_operand[1]-48,6,rs,'+');
            int_to_binary(second_operand[1]-48,6,rt,'+');

            sprintf(word, "%s%s%s", rs,rt,are);
            fprintf(fp,"%s\n",word);
            L++;
        }
        else{
            if(first_operand_type==3){
                int_to_binary(first_operand[1]-48,6,rs,'+');
                sprintf(word, "%s%s%s", rs,rt,are);
                fprintf(fp,"%s\n",word);
                L++;

                value = searchWithSpaces(ht, second_operand);
                if(second_operand_type>=4){
                    int_to_binary(second_operand_type-4,12,number,second_operand[1]);
                    sprintf(word, "%s%s", number,are);
                    fprintf(fp,"%s\n",word);
                }
                else if(value==-2) {
                    fprintf(fp, "%s\n", "00000000000001");
                    L++;
                    efp = fopen(externfilepath, "a");
                    fprintf(efp,"%s\t%d\n",second_operand,L+100);
                    fclose(efp);

                }else {
                    int_to_binary(value, 12, labels, '+');
                    fprintf(fp, "%s%s\n", labels, "10");
                    L++;

                }

            }
            else if(first_operand_type==1){

                value = searchWithSpaces(ht, first_operand);

                if(value==-2) {
                    fprintf(fp, "%s\n", "00000000000001");
                    efp = fopen(externfilepath, "a");
                    fprintf(efp,"%s\t%d\n",first_operand,L+100);
                    fclose(efp);
                    L++;

                }else {
                    int_to_binary(value, 12, labels, '+');
                    fprintf(fp, "%s%s\n", labels, "10");
                    L++;
                }
                if(second_operand_type==1){

                    value = searchWithSpaces(ht, second_operand);

                    if(value==-2) {
                        fprintf(fp, "%s\n", "00000000000001");
                        efp = fopen(externfilepath, "a");
                        fprintf(efp,"%s\t%d\n",second_operand,L+100);
                        fclose(efp);
                        L++;

                    }else {
                        int_to_binary(value, 12, labels2, '+');
                        fprintf(fp, "%s%s\n", labels2, "10");
                        L++;
                    }


                }else{
                    int_to_binary(second_operand[1]-48,6,rt,'+');
                    sprintf(word, "%s%s%s", rs,rt,are);
                    fprintf(fp,"%s\n",word);
                    L++;
                }
            } else {

                int_to_binary(first_operand_type-4,12,number,first_operand[1]);
                sprintf(word, "%s%s", number,are);
                fprintf(fp,"%s\n",word);
                if(second_operand_type==1){

                    value = searchWithSpaces(ht, second_operand);

                    if(value==-2) {
                        fprintf(fp, "%s\n", "00000000000001");
                        efp = fopen(externfilepath, "a");
                        fprintf(efp,"%s\t%d\n",second_operand,L+100);
                        fclose(efp);
                        L++;

                    }else {
                        int_to_binary(value, 12, labels2, '+');
                        fprintf(fp, "%s%s\n", labels2, "10");
                        L++;
                    }

                }else{
                    int_to_binary(second_operand[1]-48,6,rt,'+');
                    sprintf(word, "%s%s%s", rs,rt,are);
                    fprintf(fp,"%s\n",word);
                    L++;
                }

            }
        }
    }
    else if(opcodenum==15 || opcodenum==14){
        sprintf(word, "%s%s%s%s%s%s", param2,param1,opcode,addressing_source,addressing_target,are);
        fprintf(fp,"%s\n",word);
        L++;
    }
    else if(opcodenum==4 ||opcodenum==5 ||opcodenum==7 ||opcodenum==8 ||opcodenum==11 ||opcodenum==12){
        char first_operand[81];
        int first_operand_type;
        remove_first_word(input);
        first_operand_type = get_first_operand_type(input, first_operand);

        int_to_binary((first_operand_type>=4)?0:first_operand_type,2,addressing_target,'+');
        sprintf(word, "%s%s%s%s%s%s", param2,param1,opcode,addressing_source,addressing_target,are);
        fprintf(fp,"%s\n",word);
        L++;

        if(first_operand_type==3){

            int_to_binary(first_operand[1]-48,6,rt,'+');
            sprintf(word, "%s%s%s", rs,rt,are);
            fprintf(fp,"%s\n",word);
            L++;
        }
        else if(first_operand_type==1){

            value = searchWithSpaces(ht, first_operand);

            if(value==-2) {
                fprintf(fp, "%s\n", "00000000000001");
                efp = fopen(externfilepath, "a");
                fprintf(efp,"%s\t%d\n",first_operand,L+100);
                fclose(efp);
                L++;
            }else {
                int_to_binary(value, 12, labels, '+');
                fprintf(fp, "%s%s\n", labels, "10");
                L++;
            }


        }
        else{
            int_to_binary(first_operand_type-4,12,number,first_operand[1]);
            sprintf(word, "%s%s", number,are);
            fprintf(fp,"%s\n",word);
            L++;
        }

    }
    else if(opcodenum==9 || opcodenum==13 || opcodenum==10){

        char before_paren[81];
        char before_paren1[81];

        remove_first_word(input);
        copyBeforeParen(input,before_paren1);


        if(remove_before_open_parentheses(input,before_paren)){
            char first_param[81];
            char second_param[81];
            int first_operand_type;
            int second_operand_type;

            first_operand_type = get_first_operand_type(input, first_param);
            remove_first_word(input);
            second_operand_type = get_first_operand_type(input, second_param);

            strcpy(addressing_target,"10");

            int_to_binary((first_operand_type>=4)?0:first_operand_type,2,param1,first_param[1]);
            remove_first_word(input);
            int_to_binary((second_operand_type>=4)?0:second_operand_type,2,param2,second_param[1]);

            sprintf(word, "%s%s%s%s%s%s", param1,param2,opcode,addressing_source,addressing_target,are);
            fprintf(fp,"%s\n",word);
            L++;

            value = searchWithSpaces(ht, before_paren1);

            if(value==-2) {
                fprintf(fp, "%s\n", "00000000000001");
                efp = fopen(externfilepath, "a");
                fprintf(efp,"%s\t%d\n",before_paren1,L+100);
                fclose(efp);
                L++;

            }
            else {
                int_to_binary(value, 12, labels3, '+');
                fprintf(fp, "%s%s\n", labels3, "10");
                L++;
            }

            if(first_operand_type==second_operand_type && first_operand_type==3){
                int_to_binary(first_param[1]-48,6,rs,'+');
                int_to_binary(second_param[1]-48,6,rt,'+');

                sprintf(word, "%s%s%s", rs,rt,are);
                fprintf(fp,"%s\n",word);
                L++;
            }
            else{
                if(first_operand_type==3){
                    int_to_binary(first_param[1]-48,6,rs,'+');
                    sprintf(word, "%s%s%s", rs,rt,are);
                    fprintf(fp,"%s\n",word);
                    L++;

                    value = searchWithSpaces(ht, second_param);

                    if(value==-2) {
                        fprintf(fp, "%s\n", "00000000000001");
                        efp = fopen(externfilepath, "a");
                        fprintf(efp,"%s\t%d\n",second_param,L+100);
                        fclose(efp);
                        L++;
                    }else {
                        int_to_binary(value, 12, labels, '+');
                        fprintf(fp, "%s%s\n", labels, "10");
                        L++;
                    }

                }
                else if(first_operand_type==1){

                    value = searchWithSpaces(ht, first_param);
                    if(value==-2) {
                        fprintf(fp, "%s\n", "00000000000001");
                        efp = fopen(externfilepath, "a");
                        fprintf(efp,"%s\t%d\n",first_param,L+100);
                        fclose(efp);
                        L++;
                    }else {
                        int_to_binary(value, 12, labels, '+');
                        fprintf(fp, "%s%s\n", labels, "10");
                        L++;

                    }

                    if(second_operand_type==1){
                        value = searchWithSpaces(ht, second_param);


                        if(value==-2) {
                            fprintf(fp, "%s\n", "00000000000001");
                            efp = fopen(externfilepath, "a");
                            fprintf(efp,"%s\t%d\n",second_param,L+100);
                            fclose(efp);
                            L++;

                        }else {
                            int_to_binary(value, 12, labels2, '+');
                            fprintf(fp, "%s%s\n", labels2, "10");
                            L++;
                        }
                    }
                    else{
                        int_to_binary(second_param[1]-48,6,rt,'+');
                        sprintf(word, "%s%s%s", rs,rt,are);
                        fprintf(fp,"%s\n",word);
                        L++;
                    }
                } else {
                    int_to_binary(first_operand_type-4,12,number,first_param[1]);
                    sprintf(word, "%s%s", number,are);
                    fprintf(fp,"%s\n",word);
                    L++;
                    if(second_operand_type==1){


                        value = searchWithSpaces(ht, second_param);

                        if(value==-2) {
                            fprintf(fp, "%s\n", "00000000000001");
                            efp = fopen(externfilepath, "a");
                            fprintf(efp,"%s\t%d\n",second_param,L+100);
                            fclose(efp);
                            L++;
                        }else {
                            int_to_binary(value, 12, labels2, '+');
                            fprintf(fp, "%s%s\n", labels2, "10");
                            L++;
                        }


                    }else{
                        int_to_binary(second_param[1]-48,6,rt,'+');
                        sprintf(word, "%s%s%s", rs,rt,are);
                        fprintf(fp,"%s\n",word);
                        L++;
                    }

                }
            }

        }
        else{

            char first_operand[81];
            int first_operand_type = get_first_operand_type(input, first_operand);
            int_to_binary((first_operand_type>=4)?0:first_operand_type,2,addressing_target,first_operand[1]);
            sprintf(word, "%s%s%s%s%s%s", param2,param1,opcode,addressing_source,addressing_target,are);
            fprintf(fp,"%s\n",word);
            L++;

            if(first_operand_type==3){

                int_to_binary(first_operand[1]-48,6,rt,'+');
                sprintf(word, "%s%s%s", rs,rt,are);
                fprintf(fp,"%s\n",word);
                L++;
            }
            else if(first_operand_type==1){


                value = searchWithSpaces(ht, input);

                if(value==-2) {
                    fprintf(fp, "%s\n", "00000000000001");
                    efp = fopen(externfilepath, "a");
                    fprintf(efp,"%s\t%d\n",input,L+100);
                    fclose(efp);
                    L++;

                }else {
                    int_to_binary(value, 12, labels, '+');
                    fprintf(fp, "%s%s\n", labels, "10");
                    L++;
                }

            }
            else{
                int_to_binary(first_operand_type-4,12,number,first_operand[1]);
                sprintf(word, "%s%s", number,are);
                fprintf(fp,"%s\n",word);
                L++;
            }
        }


    }
    else if(strncmp(input, ".string", 7)==0){

        remove_first_word(input);
        L+=print_ascii_codes(input,fp);
    }
    else if(strncmp(input, ".data", 5)==0) {
        remove_first_word(input);

        separate_numbers(input,fp);

    }
    return L;
}
int starts_with_data(char* str) {
    char* substr = ".data";
    char* ptr = strstr(str, substr);
    return (ptr != NULL && ptr == str + (isspace(*(ptr-1)) ? 1 : 0));
}


/**
* Executes the second stage of the assembler program.
* Reads the temporary file, removes labels if exist, and translates the assembly lines to machine code.
* Writes the machine code to the output file and updates the instruction and data counters.
* @param ht pointer to the symbol table hash table
* @param entryht pointer to the entry symbol table hash table
* @param filetmp path to the temporary file containing the assembly code
* @param filepath path to the output file to write the machine code to
* @param externfilepath path to the output file for external symbols
* @return void
*/
int secondStage(HashTable* ht,HashTable* enteryht,char* filetmp,char* filepath,char* externfilepath,char* entryfilepath){
    char str[100];

    int IC=0;
    int DC=0;
    FILE* file = fopen(filetmp, "r");


    FILE *fp;
    fp = fopen(filepath,"w");

    while (fscanf(file, "%[^\n]\n", str) == 1) {

        if(remove_labell(str, IC,enteryht,entryfilepath))
            remove_first_word(str);

        IC=translate_assembly_line(str, fp,ht,IC,externfilepath);



    }
    fclose(fp);
    fclose(file);
    return 0;
}
