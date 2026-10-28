#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>


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


// переводит в double
status read_double(const char *str, double *val) {
    if (val == NULL || str == NULL) return ERR_NULL;

    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return ERR_INVALID;

    char *endptr;
    *val = strtod(str, &endptr);

    if (endptr == str) return ERR_INVALID;

    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) return ERR_INVALID;
        endptr++;
    }

    if (isinf(*val) || isnan(*val)) return ERR_INVALID;

    return SUCCESS;
}


// проверка системы счисления (2-36)
status validate_base(int base) {
    if (base < 2 || base > 36) return ERR_INVALID_BASE;
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


// поиск наибольшего общего делителя алгоритмом евклида
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


status to_frac(double x, long long *out_P, long long *out_Q) {
    if (out_P == NULL || out_Q == NULL) return ERR_NULL;

    long long multiplier = 10000000000000000LL; // 10^16 - ориентировалась на 0.1
    long long P = (long long)round(x * multiplier);
    long long Q = multiplier;

    long long common = gcd(P, Q);
    
    *out_P = P / common;
    *out_Q = Q / common;
    
    return SUCCESS;
}


// проверка конечности для 1 числа - если все множители из разложения знаменателя на 
// простые множители есть в разложении основания, то дробь конечная
int is_finite_num(long long Q, int base) {
    long long common = 0;

    while (1) {
        common = gcd(Q, base);
        if (common == 1) break;
        Q = Q / common; // сокращаем - убираем этот делитель
    }

    return (Q == 1) ? 1 : 0;
}


// проверка для всех - основная функция
status check(int base, int count, ...) {
    if (count < 1 || count > MAX_NUMBERS) return ERR_INVALID_COUNT;
    if (base < 2 || base > 36) return ERR_INVALID_BASE;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i) {
        double num = va_arg(args, double);

        long long P, Q;
        status frac_res = to_frac(num, &P, &Q);
        if (frac_res != SUCCESS) {
            printf("Ошибка обработки числа %.6f\n", num);
            continue;
        }
        
        int is_finite = is_finite_num(Q, base);
        
        printf("Число %.6f: ", num);
        if (is_finite == 1) printf("имеет конечное представление в системе счисления с основанием %d.\n", base);
        else printf("имеет бесконечное представление в системе счисления с основанием %d.\n", base);
    }

    va_end(args);
    return SUCCESS;
}
