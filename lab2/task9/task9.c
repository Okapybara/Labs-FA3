#include "functions9.h"

int main() {
    int base = 0;
    int count = 0;
    char *input_str = NULL;
    int input_len = 0;
    double numbers[MAX_NUMBERS];

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
        printf("Введите количество чисел для проверки (от 1 до %d): ", MAX_NUMBERS);
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
            printf("Введите число %d (строго в диапазоне от 0 до 1): ", i + 1);
            status res = read_string(&input_str, &input_len);
            if (res != SUCCESS) {
                print_error(res);
                return 1;
            }

            if (input_len == 0) {
                printf("Ввод не может быть пустым.\n");
                free(input_str);
                continue;
            }

            double temp_val = 0.0;
            if (read_double(input_str, &temp_val) == SUCCESS && temp_val > 0.0 && temp_val < 1.0) {
                numbers[i] = temp_val;
                free(input_str);
                break;
            }
        
            printf("Неверное число. Попробуйте снова.\n");
            free(input_str);
        }
    }

    status res = check(base, count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4],
                                          numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]);

    if (res != SUCCESS) print_error(res);

    return 0;
}
