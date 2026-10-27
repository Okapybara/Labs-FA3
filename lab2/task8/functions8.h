#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>


typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID, // наличие неверных значений
    ERR_NULL, // указатель нулевой
    ERR_OVERFLOW, // переполнение
    ERR_INVALID_BASE, // неверное основание
    ERR_MEMORY, // ошибка выделения памяти
    ERR_INVALID_COUNT // неверное количество чисел введено
} status;

#define MAX_NUMBERS 10
#define INITIAL_BUF_SIZE 1024

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_INVALID_BASE: printf("Основание системы счисления должно быть от 2 до 36.\n"); break;
        case ERR_MEMORY: printf("Ошибка выделения памяти.\n"); break;
        case ERR_INVALID_COUNT: printf("Количество чисел должно быть от 1 до %d.\n", MAX_NUMBERS); break;            
        default: printf("Неизвестная ошибка.\n"); break;
    }
}

// переполнение для количества чисел + основания системы счисления
status Overflow_INT(const char *str){
    if (str == NULL) return ERR_NULL;

    char str_int_max[20];
    snprintf(str_int_max, sizeof(str_int_max), "%d", INT_MAX);

    while (str[0] == '0' && str[1] != '\0') str++;

    size_t len_str = strlen(str);
    size_t len_max = strlen(str_int_max);

    int is_over = 0;

    if (len_str > len_max) 
        is_over = 1;
    else if (len_str == len_max && strcmp(str, str_int_max) > 0)
        is_over = 1;

    if (is_over) return ERR_OVERFLOW;

    return SUCCESS;
}


// перевод в целый тип для количества чисел  + основания системы счисления
status read_int(const char *str, int *out_val) {
    if (str == NULL || out_val == NULL) return ERR_NULL;

    if (str[0] == '\0') return ERR_INVALID;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return ERR_INVALID;
    }

    status overflow = Overflow_INT(str);
    if (overflow != SUCCESS) return overflow;

    *out_val = atoi(str);

    return SUCCESS;
}

// считывает данные
status read_string(char **out_str, int *out_len) {
    if (out_str == NULL || out_len == NULL) return ERR_NULL;

    int capacity = INITIAL_BUF_SIZE;
    int length = 0;
    char *buffer = (char *)malloc(capacity * sizeof(char));
    if (buffer == NULL) return ERR_MEMORY;

    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') { // читаем пока не конец строки и не enter
        if (ch == '\r') continue;

        if (length >= capacity - 1) {
            if (capacity > INT_MAX / 2) {
                free(buffer);
                return ERR_OVERFLOW;
            }
            capacity *= 2;
            char *new_buffer = (char *)realloc(buffer, capacity * sizeof(char));
            if (new_buffer == NULL) {
                free(buffer);
                return ERR_MEMORY;
            }
            buffer = new_buffer;
        }
        buffer[length++] = (char)ch;
    }
    
    buffer[length] = '\0';
    *out_str = buffer;
    *out_len = length;
    return SUCCESS;
}


// букву в число
int char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

// число в букву
char val_to_char(int v) {
    if (v >= 0 && v <= 9) return (char)('0' + v);
    return (char)('A' + (v - 10));
}

// проверка системы счисления (2-36)
status validate_base(int base) {
    if (base < 2 || base > 36) return ERR_INVALID_BASE;
    return SUCCESS;
}

// проверка введеного числа
status validate_num_str(const char *str, int base) {
    if (str == NULL || str[0] == '\0') return ERR_INVALID;
    
    for (int i = 0; str[i] != '\0'; i++) {
        int val = char_to_val(str[i]);
        if (val < 0 || val >= base) return ERR_INVALID;
    }
    return SUCCESS;
}

// удаление ведущих нулей
status delete_leading_zeros(char *number) {
    if (number == NULL) return ERR_NULL;
    
    int start_idx = 0;
    while (number[start_idx] == '0') start_idx++;

    if (number[start_idx] == '\0') {
        number[0] = '0';
        number[1] = '\0';
    }

    else {
        int i = 0;
        while (number[start_idx] != '\0') {
            number[i] = number[start_idx];
            i++;
            start_idx++;
        }
        number[i] = '\0';
    }

    return SUCCESS;
}


// сложение в столбик в определенной системе счисления
status sum_str_in_base(const char *num1, const char *num2, int base, char **out_result) {
    if (num1 == NULL || num2 == NULL || out_result == NULL) return ERR_NULL;

    int len1 = strlen(num1);
    int len2 = strlen(num2);

    int max_len = (len1 > len2 ? len1 : len2) + 2; // +1 на случай переноса, +1 для \0
    char *result = (char *)malloc(max_len * sizeof(char));
    if (result == NULL) return ERR_MEMORY;

    int i = len1 - 1;      // конец 1 строки
    int j = len2 - 1;      // конец 2 строки
    int k = max_len - 1;   // конец буфера результата
    int carry = 0;         // перенос в старший разряд

    result[k] = '\0';
    k--;

    while (i >= 0 || j >= 0 || carry > 0) {
        int val1 = (i >= 0) ? char_to_val(num1[i]) : 0; // если индекс вышел за границу то цифра = 0
        int val2 = (j >= 0) ? char_to_val(num2[j]) : 0;

        int sum = val1 + val2 + carry;
        
        carry = sum / base;
        int current_digit = sum % base;

        result[k] = val_to_char(current_digit);
        
        k--;
        i--;
        j--;
    }

    int read_pos = k + 1; // откуда коопируем
    int write_pos = 0; // куда копируем - сдвигаем все к 0 индексу
    
    while (result[read_pos] != '\0') {
        result[write_pos] = result[read_pos];
        write_pos++;
        read_pos++;
    }
    result[write_pos] = '\0';

    delete_leading_zeros(result);

    *out_result = result;
    return SUCCESS;
}


// сложение переданных чисел - функция задания
status sum_numbers(int base, int count, char **out_result, ...) {
    if (out_result == NULL) return ERR_NULL;
    if (validate_base(base) != SUCCESS) return ERR_INVALID_BASE;
    if (count < 1 || count > MAX_NUMBERS) return ERR_INVALID_COUNT;

    va_list args;
    va_start(args, out_result);

    status res = SUCCESS;
    char *current_sum = NULL;

    for (int i = 0; i < count; ++i) {
        char *num = va_arg(args, char *);
        
        if (validate_num_str(num, base) != SUCCESS) {
            res = ERR_INVALID;
            break; 
        }

        // первое число - копируем
        if (i == 0) {
            current_sum = (char *)malloc((strlen(num) + 1) * sizeof(char));
            if (current_sum == NULL) {
                res = ERR_MEMORY;
                break;
            }
            strcpy(current_sum, num);
        }
        else {
            char *new_sum = NULL;
            status sum_res = sum_str_in_base(current_sum, num, base, &new_sum);
            
            if (sum_res != SUCCESS) {
                res = sum_res;
                break;
            }

            free(current_sum);
            current_sum = new_sum;
        }
    }

    va_end(args);

    if (res != SUCCESS) {
        free(current_sum);
        *out_result = NULL;
    } 
    else *out_result = current_sum;

    return res;
}
