#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID, // наличие неверных значений
    ERR_NULL, // указатель нулевой
    ERR_OVERFLOW, // переполнение
    ERR_MEMORY, // ошибка выделения памяти
} status;


#define MAX_VECTORS 10
#define MAX_NORMS 5
#define MAX_P 100
#define MAX_N 100
#define EPSILON 1e-9
#define INITIAL_BUF_SIZE 1024


typedef status (*NormFunc)(const double *coords, int n, void *params, double *out_norm); // универсальный тип указателя на функцию

// для хранения вектора
typedef struct Vector {
    double *coords;
    int n;
} Vector;

// для хранения норм
typedef struct Norm {
    NormFunc func; // указатель на функцию
    void *params; // доп параметры - типо матрица А или p
    char *name; // имя для вывода
} Norm;

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_MEMORY: printf("Ошибка выделения памяти.\n"); break;        
        default: printf("Неизвестная ошибка.\n"); break;
    }
}


// переполнение
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


// перевод в целый тип
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


status norm_inf(const double *coords, int n, void *params, double *out_norm) {
    if (coords == NULL || out_norm == NULL) return ERR_NULL;
    if (n <= 0) return ERR_INVALID;
    (void)params; // неиспользуемый параметр
    
    double max = fabs(coords[0]);
    for (int i = 1; i < n; i++) {
        double val = fabs(coords[i]);
        if (val > max) max = val;
    }

    *out_norm = max;
    return SUCCESS;
}

status norm_p(const double *coords, int n, void *params, double *out_norm) {
    if (coords == NULL || out_norm == NULL || params == NULL) return ERR_NULL;
    if (n <= 0) return ERR_INVALID;
    int p = *(int *)params; 
    
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double term = pow(fabs(coords[i]), (double)p);
        if (isinf(term) || isnan(term)) return ERR_OVERFLOW;
        sum += term;
        if (isinf(sum) || isnan(sum)) return ERR_OVERFLOW;
    }

    double res = pow(sum, 1.0 / (double)p);
    if (isinf(res) || isnan(res)) return ERR_OVERFLOW;

    *out_norm = res;
    return SUCCESS;
}

status norm_A(const double *coords, int n, void *params, double *out_norm) {
    if (coords == NULL || out_norm == NULL || params == NULL) return ERR_NULL;
    if (n <= 0) return ERR_INVALID;

    double **A = (double **)params;
    double *Ax = (double *)calloc(n, sizeof(double));
    if (Ax == NULL) return ERR_MEMORY;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Ax[i] += A[i][j] * coords[j];
        }
    }
    
    double Ax_x = 0.0;
    for (int i = 0; i < n; i++) {
        Ax_x += Ax[i] * coords[i];
    }
    free(Ax);

    if (isinf(Ax_x) || isnan(Ax_x)) return ERR_OVERFLOW;
    if (Ax_x < 0.0) Ax_x = 0.0;

    double res = sqrt(Ax_x);
    if (isinf(res) || isnan(res)) return ERR_OVERFLOW;

    *out_norm = res;
    
    return SUCCESS;
}


// основная
status find_longest_vectors(int n, int vec_count, int norm_count, ...) {
    if (n <= 0 || vec_count <= 0 || norm_count <= 0) return ERR_INVALID;
    if (vec_count > MAX_VECTORS || norm_count > MAX_NORMS) return ERR_INVALID;
    
    
    va_list args;
    va_start(args, norm_count);

    Vector *vecs[MAX_VECTORS];
    for (int i = 0; i < MAX_VECTORS; i++) {
        vecs[i] = va_arg(args, Vector *);
        if (i < vec_count) {
            if (vecs[i] == NULL || vecs[i]->coords == NULL) { 
                va_end(args);
                return ERR_NULL;
            }
            if (vecs[i]->n != n) {
                va_end(args);
                return ERR_INVALID;
            }
        }
    }

    Norm *norms[MAX_NORMS];
    for (int i = 0; i < MAX_NORMS; i++) {
        norms[i] = va_arg(args, Norm *);
        if (i < norm_count) {
            if (norms[i] == NULL || norms[i]->func == NULL || norms[i]->name == NULL) {
                va_end(args);
                return ERR_NULL;
            }
        }
    }

    va_end(args);

    // для записи результатов функций - норм
    double *norm_values = (double *)malloc(vec_count * sizeof(double));
    if (norm_values == NULL) return ERR_MEMORY;

    printf("Результаты вычисления норм:\n");

    for (int k = 0; k < norm_count; k++) {
        printf("\n");
        printf("Норма: %s\n", norms[k]->name);

        // вычисляем нормы и ищем максимальную норму
        double max_norm = -1.0;

        for (int i = 0; i < vec_count; i++) {
            status res = norms[k]->func(vecs[i]->coords, n, norms[k]->params, &norm_values[i]);
            if (res != SUCCESS) {
                free(norm_values);
                return res;
            }

            if (norm_values[i] > max_norm) max_norm = norm_values[i];
        }

        // вывод самых длинных
        printf("Максимальное значение нормы: %.6f\n", max_norm);
        printf("Самый(e) длинный(e) вектор(ы):\n");
        int found = 0;
        for (int i = 0; i < vec_count; i++) {
            if (fabs(norm_values[i] - max_norm) <= EPSILON) {
                printf("Вектор %d: (", i + 1);

                for (int j = 0; j < n; j++) 
                    printf("%.4f%s", vecs[i]->coords[j], (j == n - 1) ? "" : ", "); // вывод координат
                printf(")\n");

                found++;
            }
        }
        
        if (found == 0) printf("Векторы не найдены.\n");
    }
    
    free(norm_values);
    return SUCCESS;
}
