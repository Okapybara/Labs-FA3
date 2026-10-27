#include "functions8.h"

int main() {
    int base = 0;
    int count = 0;
    char *input_str = NULL;
    int input_len = 0;
    char *numbers[MAX_NUMBERS];

    // ввод основания системы счисления
    while (1) {
        printf("Введите систему счисления (от 2 до 36): ");
        status res = read_string(&input_str, &input_len);
        if (res != SUCCESS) {
            print_error(res);
            return 1;
        }

        if (input_len > 0 && read_int(input_str, &base) == SUCCESS && validate_base(base) == SUCCESS) {
            free(input_str);
            break;
        }
        
        printf("Неверное основание. Попробуйте снова.\n\n");
        free(input_str);
    }

    // ввод количества чисел
    while (1) {
        printf("Введите количество чисел для суммирования (от 1 до %d): ", MAX_NUMBERS);
        status res = read_string(&input_str, &input_len);
        if (res != SUCCESS) {
            print_error(res);
            return 1;
        }

        if (input_len > 0 && read_int(input_str, &count) == SUCCESS && count >= 1 && count <= MAX_NUMBERS) {
            free(input_str);
            break;
        }
        
        printf("Неверное количество. Попробуйте снова.\n\n");
        free(input_str);
    }

    printf("\n");

    // ввод чисел
    for (int i = 0; i < count; ++i) {
        while (1) {
            printf("Введите число %d (в системе счисления с основанием %d): ", i + 1, base);
            status res = read_string(&input_str, &input_len);
            if (res != SUCCESS) {
                print_error(res);
                for (int j = 0; j < i; ++j) free(numbers[j]);
                return 1;
            }

            if (input_len == 0) {
                printf("Ввод не может быть пустым.\n");
                free(input_str);
                continue;
            }

            if (validate_num_str(input_str, base) == SUCCESS) {
                numbers[i] = input_str; 
                break;
            } 
            else {
                printf("Число содержит недопустимые символы для основания %d.\n", base);
                free(input_str);
            }
        }
    }

    char *result = NULL;
    status res = sum_numbers(base, count, &result, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], 
                             numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);

    if (res == SUCCESS) {
        printf("\n");
        printf("Результат суммирования: %s\n", result);
    } 
    else {
        printf("\n");
        print_error(res);
    }

    free(result);
    for (int i = 0; i < count; ++i) free(numbers[i]);

    return 0;
}
