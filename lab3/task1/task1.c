#include "functions1.h"

int main(int argc, char *argv[]){
    if (argc != 3){
        print_error(ERR_INVALID_ARGC);
        printf("Использование: %s <число в 10 сс> <r = 1, ..., 5>\n", argv[0]);
        return 1;
    }

    int number = 0;
    int r = 0;

    status status_num = validate_positive_int(argv[1], &number);
    if (status_num != SUCCESS) {
        print_error(status_num);
        return 1;
    }

    status status_r = validate_positive_int(argv[2], &r);
    if (status_r != SUCCESS) {
        print_error(status_r);
        return 1;
    }

    if (r < 1 || r > 5) {
        printf("Параметр r должен быть в диапазоне от 1 до 5.\n");
        return 1;
    }

    char result_buffer[64];

    status convert_status = convert_to_base_pow2((unsigned int)number, r, result_buffer);
    if (convert_status != SUCCESS) {
        print_error(convert_status);
        return 1;
    }
    
    printf("Число %d в системе счисления с основанием 2^%d (%d): %s\n", number, r, 1 << r, result_buffer);

    return 0;
}
