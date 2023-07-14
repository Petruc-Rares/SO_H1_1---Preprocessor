#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "hashmap.h"

typedef struct helperKeyValue {
    char *key;
    char *value;
} helperKeyValue;

typedef struct argumentsExecutable {
    // name & number of the dirs that files included
    // by INFILE will be searched in
    char **DIRS;
    int no_dirs;
    // will write the preprocessed output
    // to this file
    char *OUTFILE;
    // if infile is missing, reading
    // will be made from stdin (INFILE will be null)
    char *INFILE;
} argumentsExecutable;

void freeArgumentsExecutables(argumentsExecutable *argumentsProgram) {
	int i;

    free(argumentsProgram->INFILE);
    free(argumentsProgram->OUTFILE);
    for (i = 0; i < argumentsProgram->no_dirs; i++) {
        free(argumentsProgram->DIRS[i]);
    }
    free(argumentsProgram->DIRS);
    free(argumentsProgram);
}

helperKeyValue extractKeyValue(char *info_symbol) {
    int length_info_symbol = strlen(info_symbol);
    helperKeyValue new_key_value;

    // check if optional value was given
    char equal_found = 0;
    int i = 0;
    for (i = 0; i < length_info_symbol; i++) {
        if (info_symbol[i] == '=') {
            equal_found = 1;
            break;
        }
    }

    // TODO - CHECK RETURN MALLOC
    new_key_value.key = (char *) malloc((i + 1) * sizeof(char));
    if (new_key_value.key == NULL) {
        exit(ENOMEM);
    }
    if (equal_found) {
        new_key_value.value = (char *) malloc((length_info_symbol - i) * sizeof(char));
    } else {
        new_key_value.value = (char *) malloc(1 * sizeof(char));
    }

    if (new_key_value.value == NULL) {
        exit(ENOMEM);
    }

    if (equal_found == 1) {
        strncpy(new_key_value.key, info_symbol, i);
        new_key_value.key[i] = '\0';
        strncpy(new_key_value.value, info_symbol + i + 1, length_info_symbol - i);
    } else {
        strncpy(new_key_value.key, info_symbol, i);
        new_key_value.key[i] = '\0';
        new_key_value.value[0] = '\0';
    }

    return new_key_value;
}

void parseArgv(int argc, char *argv[], argumentsExecutable *argumentsProgram, Hashmap *symbols) {
    // if "-" is encountered
    // we expect D, I or o to follow,
    // even attached like "-D", or in two
    // separate words "- D"
	int arg_no;

    // if "-" is followed by any other charac
    // than D, I, O, that will be a bad param
    for (arg_no = 1; arg_no < argc; arg_no++) {
        //printf("argv[%d] = %s\n", arg_no, argv[arg_no]);
        char *crt_argv = argv[arg_no];

        int length_crt_argv = strlen(crt_argv);
        if (length_crt_argv <= 0) {
            printf("Unexpected argument\n");
            continue;
        }

        if (crt_argv[0] == '-') {
            if (length_crt_argv <= 1) {
                printf("Unexpected argument\n");
                continue;
            }
            // check for -D <SYMBOL>[=<MAPPING>] or -D<SYMBOL>[=<MAPPING>]
            if (crt_argv[1] == 'D') {
                // symbol to follow

                char *key;
                char *value;

                // argv[arg_no + 1] is <SYMBOL>[=<MAPPING>]
                if (length_crt_argv == 2) {

                    char *info_symbol= argv[arg_no + 1];

                    helperKeyValue key_value_pair = extractKeyValue(info_symbol);

                    key = strdup(key_value_pair.key);
                    if (key == NULL) {
                        exit(ENOMEM);
                    }
                    value = strdup(key_value_pair.value);
                    if (value == NULL) {
                        exit(ENOMEM);
                    }

                    free(key_value_pair.key);
                    free(key_value_pair.value);

                   // printf("key: %s, value: %s\n", key, value);

                    arg_no++;
                } else {
                    // argv[arg_no] also contains info about the symbol
					helperKeyValue key_value_pair;
                    char *info_symbol= (char *) malloc(length_crt_argv - 2 + 1);
                    if (info_symbol == NULL) {
                        exit(ENOMEM);
                    }

                    strncpy(info_symbol, crt_argv + 2, length_crt_argv - 2);
                    info_symbol[length_crt_argv - 2] = '\0';

                    key_value_pair = extractKeyValue(info_symbol);

                    key = strdup(key_value_pair.key);
                    if (key == NULL) {
                        exit(ENOMEM);
                    }

                    value = strdup(key_value_pair.value);
                    if (value == NULL) {
                        exit(ENOMEM);
                    }

                    free(key_value_pair.key);
                    free(key_value_pair.value);
                    free(info_symbol);

                //    printf("key: %s, value: %s\n", key, value);
                }

                insert(symbols, key, value);
                free(key);
                free(value);
            }
            // check for -I <DIR> or -I<DIR>
            else if (crt_argv[1] == 'I') {
                // directory to follow
                char *directory_name;

                // case separate argv: -I <DIR>
                if (length_crt_argv == 2) {

                    char *info_symbol= argv[arg_no + 1];

                    directory_name = strdup(argv[arg_no + 1]);
                    if (directory_name == NULL) {
                        printf("Error at allocating directory name\n");
                        exit(ENOMEM);
                    }

                    arg_no++;
                }
                // case crt_argv holds also info about the directory name
                // -I<DIR>
                else {
                    directory_name = (char *) calloc(length_crt_argv - 2, sizeof(char));
                    if (directory_name == NULL) {
                        exit(ENOMEM);
                    }

                    strncpy(directory_name, crt_argv + 2, length_crt_argv - 2);
                }

                argumentsProgram->no_dirs++;
                if(argumentsProgram->DIRS == NULL) {
                    argumentsProgram->DIRS = (char **) calloc(argumentsProgram->no_dirs, sizeof(char *));

                    if (argumentsProgram->DIRS == NULL) {
                        printf("dirs in argumentsProgram failed calloc\n");
                        exit(ENOMEM);
                    }
                } else {
                    // TODO realloc
                    argumentsProgram->DIRS = realloc(argumentsProgram->DIRS, argumentsProgram->no_dirs * sizeof(char *));
                    if (argumentsProgram->DIRS == NULL) {
                        printf("error at realloc of dirs");
                        exit(ENOMEM);
                    }
                }
                argumentsProgram->DIRS[argumentsProgram->no_dirs- 1] = strdup(directory_name);
                if (argumentsProgram->DIRS[argumentsProgram->no_dirs- 1] == NULL) {
                    exit(ENOMEM);
                }

                free(directory_name);
            }
            // check for -o <OUTFILE> or -o<OUTFILE>
            else if (crt_argv[1] == 'o') {
                // name of output file to follow

                // case separate argv: -o <OUTFILE>
                if (length_crt_argv == 2) {

                    char *info_symbol= argv[arg_no + 1];

                    argumentsProgram->OUTFILE = strdup(argv[arg_no + 1]);
                    if (argumentsProgram->OUTFILE == NULL) {
                        printf("Error at allocating output file name\n");
                        exit(ENOMEM);
                    }

                    //printf("output file name v1: %s\n", output_file_name);
                    arg_no++;
                    continue;
                }
                // case crt_argv holds also info about the directory name
                // -o<OUTFILE>
                else {
                    argumentsProgram->OUTFILE = (char *) calloc(length_crt_argv - 2, sizeof(char));
                    
                    if (argumentsProgram->OUTFILE == NULL) {
                        printf("Error at allocating output file name\n");
                        exit(ENOMEM);
                    }

                    strncpy(argumentsProgram->OUTFILE, crt_argv + 2, length_crt_argv - 2);
                    //printf("output file name v2: %s\n", output_file_name);
                }
            } else {
                exit(1);
            }
        } else {
            // [<INFILE>]
            // if infile was already completed, error
            if (argumentsProgram->INFILE != NULL) {
                printf("infile already completed\n");
                exit(1);
            }

            argumentsProgram->INFILE= strdup(crt_argv);

            if (argumentsProgram->INFILE == NULL) {
                printf("Error at allocating input file name\n");
                exit(ENOMEM);
            }

           // printf("input file name: %s\n", input_file_name);
        }
    }

    return ;
}

void printArgumentsProgram(argumentsExecutable *argumentsProgram) {
	int it;

    printf("INFILE: %s\n", argumentsProgram->INFILE);
    printf("OUTFILE: %s\n", argumentsProgram->OUTFILE);
    printf("DIRS TO SEARCH IN FOR HEADER FILES, in number of: %d\n", argumentsProgram->no_dirs);
    for (it = 0; it < argumentsProgram->no_dirs; it++) {
        printf("%s\n", argumentsProgram->DIRS[it]);
    }
}

int minVal(int a, int b) {
    return (a < b) ? a : b;
}

typedef struct infoIf infoIf;
typedef struct infoIfs infoIfs;

struct infoIf {
    // true if block
    // inside an if or elif
    // was executed

    // used to avoid getting
    // in blocks of an if - else if
    // twice
    char if_executed;

    // true, if it is inside an
    // if block
    // false, if the condition
    // of the if was false
    char to_execute;


    // if executed now
    char if_execute;
};

struct infoIfs {
    infoIf *info_if;
    int crt_no;
};

void set_if_executed(infoIfs *info_Ifs) {
    info_Ifs->info_if[info_Ifs->crt_no - 1].if_executed = 1;
}

void set_if_execute(infoIfs *info_Ifs) {
    info_Ifs->info_if[info_Ifs->crt_no - 1].if_execute = 1;
}

void set_to_execute(infoIfs *info_Ifs) {
    info_Ifs->info_if[info_Ifs->crt_no - 1].to_execute = 1;
}

void reset_to_execute(infoIfs *info_Ifs) {
    info_Ifs->info_if[info_Ifs->crt_no - 1].to_execute = 0;
}

void reset_if_execute(infoIfs *info_Ifs) {
    info_Ifs->info_if[info_Ifs->crt_no - 1].if_execute = 0;
}

char get_if_executed(infoIfs *info_Ifs) {
    if (info_Ifs->crt_no == 0) return 1;

    return (info_Ifs != NULL) ? info_Ifs->info_if[info_Ifs->crt_no - 1].if_executed : -1;
}

char get_if_execute(infoIfs *info_Ifs) {
    if (info_Ifs->crt_no == 0) return 1;

    return (info_Ifs != NULL) ? info_Ifs->info_if[info_Ifs->crt_no - 1].if_execute : -1;
}

char get_to_execute(infoIfs *info_Ifs) {
    if (info_Ifs->crt_no == 0) return 1;

    return (info_Ifs != NULL) ? info_Ifs->info_if[info_Ifs->crt_no - 1].to_execute : -1;
}

void printDebugInfoIf(infoIf *info_If) {
    printf("if_executed: %hhu ", info_If->if_executed);
    printf("to_execute: %hhu \n", info_If->to_execute);
}

void printDebugInfoIfs(infoIfs *info_Ifs) {
    int it;
    printf("There currently are %d nested ifs\n", info_Ifs->crt_no);

    for (it = 0; it < info_Ifs->crt_no; it++) {
        printDebugInfoIf(&info_Ifs->info_if[it]);
    }
}

char *replace_string(char *string, char *key, char *value) { 
    char crt_char;
    char ignore;
    char length_string = strlen(string);
    int start_pos;
    int it;
    // index in key
    int index;
    char buffer_test[256];
    char buffer_result[256];

    memset(buffer_test, 0, 256);
    memset(buffer_result, 0, 256);

    ignore = 0;
    start_pos = 0;
    index = 0;


    for (it = 0; it < length_string; it++) {
        memset(buffer_test, 0, 256);
        crt_char = string[it];

        if (crt_char == '"') {
            ignore = (ignore == 0) ? 1 : 0;
        }

        if (ignore == 0) {
            if (crt_char == key[index]) {

                if (index == 0) {
                    start_pos = it;
                }

                index++;
            } else {
                memcpy(buffer_test, string + start_pos, it - start_pos + 1);
                //fprintf(stdout, "%s", buffer_test);
                sprintf(buffer_result + strlen(buffer_result), "%s", buffer_test);

                start_pos += it - start_pos + 1;
                index = 0;
            }
        } else {
            sprintf(buffer_result + strlen(buffer_result), "%c", crt_char);
            start_pos = it + 1;
        }


        if (index == strlen(key)) {
            // check if it is not just a substring
            if ((it == strlen(string) - 1) ||
                (string[it + 1] == ' ') ||
                (string[it + 1] == ')') ||
                (string[it + 1] == ',')) {
                sprintf(buffer_result + strlen(buffer_result), "%s", value);   
                index = 0;
                start_pos = it + 1;
            } else {
                sprintf(buffer_result + strlen(buffer_result), "%s", value);   
                index = 0;
                start_pos = it + 1;
            }
        }
    }


    return strdup(buffer_result);   
}

void fill_array_symbols(char **symbols_array, Hashmap *symbols) {
    int it;
    int crt_pos = 0;
    for (it = 0; it < symbols->noBuckets; it++) {
        LinkedListNode *crt_node;
        LinkedList *entry = symbols->entries[it];

        if (entry == NULL) continue;
        crt_node = entry->head;
        while (crt_node) {
            symbols_array[crt_pos] =crt_node->key;
            crt_node = crt_node->next;
            crt_pos++;
        }

    }
}

char * get_directory_input (char *buffer) {
    char *test = strrchr(buffer, '/');

    // input file is in this directory
    if (test == NULL) {
        return NULL;
    }

    
    buffer[test - buffer + 1] = '\0';
    return buffer;
}

void preprocessLine(FILE *fd_output, FILE *fd, Hashmap * symbols,
                    char *input_file_name, argumentsExecutable *argumentsProgram) {
    char buffer[256];
    int size_buffer = sizeof(buffer);

    infoIfs info_ifs;
    info_ifs.crt_no = 0;
    info_ifs.info_if = NULL;


    while (fgets(buffer, size_buffer, fd) != NULL) {
        if (get_to_execute(&info_ifs) == 0) continue;

        if(strncmp(buffer, "#define ", minVal(strlen(buffer), strlen("#define "))) == 0) {
            if ((get_to_execute(&info_ifs) == 1) && (get_if_execute(&info_ifs) == 1)) {
            
            char key[256];
            char value[256];
            char *formatted_value;
            char final_value[256];
            char *token;
            memset(key, 0, sizeof(key));
            memset(value, 0, sizeof(value));

            sscanf(buffer, "#define %s %[^\n]s", key, value);

            // now we should replace values that contain other defines in them
            // also, watch out for multi-line define
            
            formatted_value = (char *) calloc(strlen(value) + 1, sizeof(char));
            if (formatted_value == NULL) {
                exit(ENOMEM);
            }

            strcpy(formatted_value, value);

            // check for multi-line define 
            if (strlen(buffer) >= 2) {
                int i;
                if (buffer[strlen(buffer) - 2] == '\\') {
                    int formatted_value_length;
                    // read until end of file or until multi line define is done
                    while ((fgets(buffer, size_buffer, fd) != NULL)
                                && ((strlen(buffer) >= 2) && (buffer[strlen(buffer) - 2] == '\\'))) {
                        int formatted_value_length = strlen(formatted_value);
                        formatted_value = (char *) realloc(formatted_value, formatted_value_length + strlen(buffer) + 1);
                        if (formatted_value == NULL) {
                            exit(ENOMEM);
                        }

                        strcpy(formatted_value + formatted_value_length, buffer);
                    }

                    // end of file is reached
                    if (buffer == NULL) {
                        return;
                    }

                    
                    // last element included in define
                    formatted_value_length = strlen(formatted_value);
                    formatted_value = (char *) realloc(formatted_value, formatted_value_length + strlen(buffer) + 1);
                    if (formatted_value == NULL) {
                        exit(ENOMEM);
                    }
                    strcpy(formatted_value + formatted_value_length, buffer);
                }

                // TODO - check more nicely if \n or \ is inside ""
                // get everything on the same line and remove '\'
                for (i = 0; i < strlen(formatted_value); i++) {
                    if (((formatted_value[i] == '\n') ||
                        (formatted_value[i] == '\\')) && (formatted_value[i - 1] != '"'))  {
                        memcpy(formatted_value + i, formatted_value + i + 1, strlen(formatted_value) - i);
                        i--;
                    }
                }


                // remove whitespaces
                for (i = 0; i < strlen(formatted_value); i++) {
                    if ((formatted_value[i] == ' ') && (formatted_value[i + 1] == ' ')) {
                        memcpy(formatted_value + i, formatted_value + i + 1, strlen(formatted_value) - i);
                        i--;
                    }
                }
            }

                // check if there is a define in define    
                memset(final_value, 0, sizeof(final_value));

                token = strtok(formatted_value, " ");
                while (token) {
                    char *value = getValue(symbols, token);

                    if (value == NULL) {
                        memcpy(final_value + strlen(final_value), token, strlen(token));
                    } else {
                        memcpy(final_value + strlen(final_value), value, strlen(value));
                    }

                    final_value[strlen(final_value)] = ' ';

                    token = strtok(NULL, " ");
                }

                // remove last ' '  added
                final_value[strlen(final_value) - 1] = '\0';
                
                insert(symbols, key, final_value);
                free(formatted_value);
            }
        } else if (strncmp(buffer, "#include", minVal(strlen(buffer), strlen("include"))) == 0) {
            char header_name[256];
            char directory_name[256];
            char *aux_directory_name = NULL;
            FILE *fd_new;
            char header_found = 0;
            int it;

            memset(directory_name, 0, sizeof(directory_name));

            sscanf(buffer, "#include %s", header_name);

            // remove " from header_name

            // remove last "
            header_name[strlen(header_name) - 1] = '\0';
            // remove first "
            memcpy(header_name, header_name + 1, strlen(header_name) - 1);
            // remove duplicate last character
            header_name[strlen(header_name) - 1] = '\0';

            // check for the path of the input_file_name
            if (input_file_name != NULL) {
                aux_directory_name = strdup(input_file_name);
                if (aux_directory_name == NULL) {
                    exit(ENOMEM);
                }
            }


            if ((get_directory_input(aux_directory_name)) != NULL) {
                strcat(directory_name, aux_directory_name);
            }

            // we should search in the current directory
            if (strlen(directory_name) == 0) {
                fd_new = fopen(directory_name, "r");

                if (fd_new != NULL) {
                    // there is still a chance that -I args might cover
                    // still not so sure about the last 4th argument
                    preprocessLine(fd_output, fd_new, symbols, header_name, argumentsProgram);
                    header_found = 1;
                    fclose(fd_new);
                }

            } else {
                
                // search for the include file in the following directory
                strcat(directory_name, header_name);

                fd_new = fopen(directory_name, "r");
                if (fd_new != NULL) {
                    //printf("cautam aici\n");
                    //printHashmap(symbols);
                    // there is still a chance that -I args might cover
                    // still not so sure about the last 4th argument
                    preprocessLine(fd_output, fd_new, symbols, directory_name, argumentsProgram);
                    
                    //printf("dupa procesare");
                    //printHashmap(symbols);
                    header_found = 1;
                    fclose(fd_new);
                }                
            }

            for (it = 0; it < argumentsProgram->no_dirs; it++) {
                memset(directory_name, 0, sizeof(directory_name));
                strcat(directory_name, argumentsProgram->DIRS[it]);
                strcat(directory_name, "/");
                strcat(directory_name, header_name);

                fd_new = fopen(directory_name, "r");
                if (fd_new != NULL) {
                    // there is still a chance that -I args might cover
                    // still not so sure about the last 4th argument
                    preprocessLine(fd_output, fd_new, symbols, directory_name, argumentsProgram);
                    header_found = 1;
                    fclose(fd_new);
                    break;
                }   
            }

            // header not found
            if (header_found == 0) {
                exit(1);
            }

            free(aux_directory_name);               

        } else if (strncmp(buffer, "#ifdef", minVal(strlen(buffer), strlen("#ifdef"))) == 0) {
			char key_ifdef[256];
			char *value;

            info_ifs.info_if = (infoIf *) realloc(info_ifs.info_if, (info_ifs.crt_no + 1) * sizeof(infoIf));
            if (info_ifs.info_if == NULL) {
                exit(ENOMEM);
            }

            info_ifs.info_if[info_ifs.crt_no].if_executed = 0;
            info_ifs.info_if[info_ifs.crt_no].if_execute = 0;
            info_ifs.info_if[info_ifs.crt_no].to_execute = get_to_execute(&info_ifs);
            info_ifs.crt_no++;

            if (get_to_execute(&info_ifs) == 0) continue;

            
            sscanf(buffer, "#ifdef %s", key_ifdef);

            value = getValue(symbols, key_ifdef);

            // symbol key_ifdef undefined
            if (value == NULL) {
                continue;
            }


            // we should execute the block in the #if
            set_if_executed(&info_ifs);
            set_if_execute(&info_ifs);

        } else if (strncmp(buffer, "#ifndef", minVal(strlen(buffer), strlen("#ifndef"))) == 0) {
			char key_ifndef[256];
			char *value;

            info_ifs.info_if = (infoIf *) realloc(info_ifs.info_if, (info_ifs.crt_no + 1) * sizeof(infoIf));
            if (info_ifs.info_if == NULL) {
                exit(ENOMEM);
            }

            info_ifs.info_if[info_ifs.crt_no].if_executed = 0;
            info_ifs.info_if[info_ifs.crt_no].if_execute = 0;
            info_ifs.info_if[info_ifs.crt_no].to_execute = get_to_execute(&info_ifs);
            info_ifs.crt_no++;

            if (get_to_execute(&info_ifs) == 0) continue;

            sscanf(buffer, "#ifndef %s", key_ifndef);

            value = getValue(symbols, key_ifndef);

            // value defined
            if (value != NULL) {
                continue;
            }

            set_if_executed(&info_ifs);
            set_if_execute(&info_ifs);

        } else if (strncmp(buffer, "#if", minVal(strlen(buffer), strlen("#if"))) == 0) {
			char key_if[256];
			char *value;

            info_ifs.info_if = (infoIf *) realloc(info_ifs.info_if, (info_ifs.crt_no + 1) * sizeof(infoIf));
            if (info_ifs.info_if == NULL) {
                exit(ENOMEM);
            }

            info_ifs.info_if[info_ifs.crt_no].if_executed = 0;
            info_ifs.info_if[info_ifs.crt_no].if_execute = 0;
            info_ifs.info_if[info_ifs.crt_no].to_execute = get_to_execute(&info_ifs);
            info_ifs.crt_no++;

            // no need to execute it
            if (get_to_execute(&info_ifs) == 0) continue;

            sscanf(buffer, "#if %s", key_if);

            value = getValue(symbols, key_if);
            // if no definition for the value
            if (value == NULL) {
                // TODO - might be a value like "#if 1"
                
                int maybe_int = atoi(key_if);

                if (maybe_int != 0) {
                    value = key_if;
                }

                // even if key_if = "0", the value
                // would be 0, but if it stays NULL
                // it is still ok
                
            }

            // check the value to be different from '0' literal
            if ((value == NULL) || (value[0] == '0')) {
                continue;
            }

            // we should execute the block in the #if
            set_if_executed(&info_ifs);
            set_if_execute(&info_ifs);

        } else if (strncmp(buffer, "#elif", minVal(strlen(buffer), strlen("#elif"))) == 0) {
            char key_if[256];
			char *value;

            // no need to execute it
            if (get_to_execute(&info_ifs) == 0) continue;

            // if an #if or #elif or #ifdef, #ifndef
            // was executed, we no longer execute
            // the following lines
            if (get_if_executed(&info_ifs)) {
                continue;
            }

            // exactly the same like at if - TODO modularize

            sscanf(buffer, "#elif %s", key_if);

            value = getValue(symbols, key_if);
            // if no definition for the value
            if (value == NULL) {
                // TODO - might be a value like "#if 1"
                //printf("it is pretty null");
                
                int maybe_int = atoi(key_if);

                if (maybe_int != 0) {
                    value = key_if;
                }

                // even if key_if = "0", the value
                // would be 0, but if it stays NULL
                // it is still ok
            }


            if ((value == NULL) || (value[0] == '0')) {
                continue;
            } else {
                // elif is true
                // executes the block only if
                // 
                if (get_if_execute(&info_ifs) == 0) {
                    set_if_execute(&info_ifs);
                    set_if_executed(&info_ifs);
                }
            }

            
        } else if (strncmp(buffer, "#else", minVal(strlen(buffer), strlen("#else"))) == 0) {
            // no need to execute it
            if (get_to_execute(&info_ifs) == 0) continue;
            
            // if an #if or #elif or #ifdef, #ifndef
            // was executed, we no longer execute
            // the following lines
            if (get_if_executed(&info_ifs)) {
                reset_if_execute(&info_ifs);
                continue;
            }

            if (get_if_execute(&info_ifs) == 0)  {
                set_if_execute(&info_ifs);
            } else {
                
            }

        } else if (strncmp(buffer, "#endif", minVal(strlen(buffer), strlen("#endif"))) == 0) {
            info_ifs.crt_no--;
            info_ifs.info_if = (infoIf *) realloc(info_ifs.info_if, info_ifs.crt_no * sizeof(infoIf));
            if ((info_ifs.info_if == NULL) && (info_ifs.crt_no > 0)) {
                exit(ENOMEM);
            }
        }
        else if (strncmp(buffer, "#undef", minVal(strlen(buffer), strlen("#undef"))) == 0) {
            //  printf("buffer de undef: %s\n", buffer);
            char key_remove[256];
            sscanf(buffer, "#undef %s\n", key_remove);
            removeKey(symbols, key_remove);           
        } else {
            if ((get_to_execute(&info_ifs) == 1) && (get_if_execute(&info_ifs) == 1)) {
                // we should parse the buffer and
                // check if there are any symbols
                // to replace
                int no_symbols;
                char **symbols_as_array;
                char *result;
                char *aux;
                int i;

                no_symbols = getNoKeys(symbols);

                symbols_as_array = (char **) calloc(no_symbols, sizeof(char *));
                if (symbols_as_array == NULL) {
                    exit(ENOMEM);
                }

                fill_array_symbols(symbols_as_array, symbols);


                result = buffer;

                for (i = 0; i < no_symbols; i++) {
                    aux = result;              
                    result = replace_string(aux, symbols_as_array[i], getValue(symbols, symbols_as_array[i]));
                    if (result == NULL) {
                        exit(ENOMEM);
                    }
                    if (i > 0) free(aux);
                }

                fprintf(fd_output, "%s", result);

                if (i > 0) free(result);
                free(symbols_as_array);
            } 
        }
    }
}

void preprocessInputFile(argumentsExecutable *argumentsProgram, Hashmap *symbols) {
    FILE *fd;
	FILE* fd_output;
    if (argumentsProgram->INFILE == NULL) {
        // no input file was given
        // in the arguments of the executable
        fd = stdin;
    } else {
        fd = fopen(argumentsProgram->INFILE, "r");
    }
    // TODO - check fd if open or not
    
    if (argumentsProgram->OUTFILE == NULL) {
        // no input file was given
        // in the arguments of the executable
        fd_output = stdout;
    } else {
        fd_output = fopen(argumentsProgram->OUTFILE, "r");
    }    
    

    if (fd == NULL) {
        exit(1);
    }

    // todo - change name
    preprocessLine(fd_output, fd, symbols, argumentsProgram->INFILE, argumentsProgram);
    if (fd != stdin)
        fclose(fd);
}

int main(int argc, char *argv[]) {

    // argc >= 1, as argv[0] holds the name of the executable
    // check if any arguments are given extra
    argumentsExecutable *argumentsProgram = (argumentsExecutable *) calloc(1, sizeof(argumentsExecutable));
	Hashmap *symbols = createHashmap(DEFAULT_NO_BUCKETS);

    if (argumentsProgram == NULL) {
        exit(ENOMEM);
    }


    argumentsProgram->no_dirs = 0;
    argumentsProgram->DIRS = NULL;
    argumentsProgram->OUTFILE = NULL;
    argumentsProgram->INFILE = NULL;

    if (argumentsProgram == NULL) {
        printf("Calloc failed in argumentsProgram allocation");
        exit(ENOMEM);
    }

    if (argc > 1) {
        parseArgv(argc, argv, argumentsProgram, symbols);
    }

    //printHashmap(symbols);
    // TODO, begin parsing the input file (if given in command line, otherwise from stdin)
    preprocessInputFile(argumentsProgram, symbols);

    //printArgumentsProgram(argumentsProgram);
    //printHashmap(symbols);

    // TODO - also free the fields inside (INFILE, OUTFILE, DIRS, HASHMAP, IDK)
    freeArgumentsExecutables(argumentsProgram);
    freeHashmap(symbols);
}