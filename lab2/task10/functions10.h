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
    ERR_INVALID_DEGREE, // неверная степень
    ERR_MEMORY, // ошибка выделения памяти
    ERR_INVALID_EPS // неверная точность эпсилон
} status;

#define MAX_DEGREE 10
#define INITIAL_BUF_SIZE 1024


// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_INVALID_DEGREE: printf("Степень многочлена должна быть >= 0 и до %d.\n", MAX_DEGREE); break;
        case ERR_MEMORY: printf("Ошибка выделения памяти.\n"); break;         
        case ERR_INVALID_EPS: printf("Неверно введена точность эпсилон. Должна быть в диапазоне (0, 1].\n"); break;
        default: printf("Неизвестная ошибка.\n"); break;
    }
}


// переполнение для степени
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


// перевод в целый тип для степени
status read_int(const char *str, int *val) {
    if (str == NULL || val == NULL) return ERR_NULL;

    if (str[0] == '\0') return ERR_INVALID;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return ERR_INVALID;
    }

    status overflow = Overflow_INT(str);
    if (overflow != SUCCESS) return overflow;

    *val = atoi(str);

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


// функция перевода
status redecomposition(double eps, double a, double **out_g, int degree, ...) {
    if (out_g == NULL) return ERR_NULL;
    if (eps <= 0.0) return ERR_INVALID_EPS;
    if (degree < 0) return ERR_INVALID_DEGREE;

    va_list args;
    va_start(args, degree);

    int n = degree;
    
    // массив исходных коэффициентов
    double *f = (double *)malloc((n + 1) * sizeof(double));
    if (f == NULL) {
        va_end(args);
        return ERR_MEMORY;
    }

    for (int i = 0; i <= n; i++) {
        f[i] = va_arg(args, double);
    }
    va_end(args);

    // для результата и для рабочего
    double *g = (double *)malloc((n + 1) * sizeof(double));
    double *b = (double *)malloc((n + 1) * sizeof(double));
    
    if (g == NULL || b == NULL) {
        free(f);
        free(g);
        free(b);
        return ERR_MEMORY;
    }

    // рабочий массив от старшей степени
    for (int i = 0; i <= n; i++) {
        b[i] = f[n - i];
    }

    for (int k = 0; k <= n; k++) {
        int current_degree = n - k;
        double current = b[0];
        
        for (int i = 1; i <= current_degree; i++) {
            current = current * a + b[i]; // схема горнера
            b[i] = current;
        }
        
        g[k] = b[current_degree];
    }

    // обнуляем значения, которые меньше эпсилон
    for (int i = 0; i <= n; i++) {
        if (fabs(g[i]) < eps) g[i] = 0.0;
    }

    free(f);
    free(b);
    *out_g = g;
    
    return SUCCESS;
}

void print_poly(double *g, double eps, int degree, double a) {
    if (g == NULL || degree < 0) return;

    int first = 1;

    for (int i = 0; i <= degree; i++) {
        if (fabs(g[i]) < eps) continue;

        if (!first) printf("%s", g[i] > 0 ? " + " : " - ");
        else {
            printf(g[i] < 0 ? "-" : "");
            first = 0;
        }

        double val = fabs(g[i]);

        if (i == 0) printf("%.2f", val);
        else {
            if (fabs(val - 1.0) > eps) printf("%.2f*", val);
            
            printf("(x");
            if (a > 0) printf(" - %.2f", a);
            else if (a < 0) printf(" + %.2f", -a);
            printf(")");
            
            if (i > 1) printf("^%d", i);
        }
    }

    if (first) printf("0");
    printf("\n");
}
