#include "functions1.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_error(ERR_INVALID_ARGC);
        return 1;
    }

    char* flag = argv[1];
    char* input_str = argv[2];

    if (flag[0] != '-' || flag[1] == '\0' || flag[2] != '\0') {
        print_error(ERR_INVALID_FLAG);
        return 1;
    }

    char flag_letter = flag[1];
    unsigned int seed = 0; // для -c
    char** strings = NULL; // массив указателей на строки
    unsigned int strs_count = 0;

    if (flag_letter == 'c') {
        if (argc < 5) {
            print_error(ERR_INVALID_ARGC);
            return 1;
        }

        strs_count = argc - 3;
        strings = (char**)malloc(strs_count * sizeof(char*));
        if (strings == NULL) {
            print_error(ERR_MEMORY);
            return 1;
        }
        
        strings[0] = (char*)argv[2];
        for (unsigned int i = 4; i < (unsigned int)argc; i++) {
            strings[i-3] = (char*)argv[i];
        }        
        
        status status_seed = unsigned_int(argv[3], &seed);
        if (status_seed != SUCCESS) {
            free(strings);
            print_error(status_seed);
            return 1;
        }
    }
    else if (flag_letter == 'l' || flag_letter == 'r' || flag_letter == 'u' || flag_letter == 'n') {
        if (argc != 3) {
            print_error(ERR_INVALID_ARGC);
            return 1;
        }
    } 
    else {
        print_error(ERR_INVALID_FLAG);
        return 1;
    }


    status res_status = SUCCESS;
    char* result_str = NULL;
    unsigned int res_len = 0;
    switch (flag_letter) {
        case 'l':
            res_status = FlagL(input_str, &res_len);
            if (res_status == SUCCESS) printf("%u\n", res_len);
            break;
        case 'r':
            res_status = FlagR(input_str, &result_str);
            break;
            
        case 'u':
            res_status = FlagU(input_str, &result_str);
            break;
            
        case 'n':
            res_status = FlagN(input_str, &result_str);
            break;
            
        case 'c':
            res_status = FlagC(strings, strs_count, seed, &result_str);
            free(strings);
            break;
            
        default:
            res_status = ERR_INVALID_FLAG;
            break;
    }

    if (res_status == SUCCESS && result_str != NULL) {
        printf("%s\n", result_str);
        free(result_str); 
    }
    else if (res_status != SUCCESS) print_error(res_status);

    return 0;
}
