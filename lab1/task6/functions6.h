#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>

typedef enum status {
    SUCCESS = 0, //успех
    ERR_NULL, // указатель нулевой
    ERR_INVALID, // наличие неверных значений
    ERR_READ, // ошибка чтения
    ERR_OVERFLOW // переполнение
} status;


// выпуклый ли многоугольник - все углы меньше 180
status func1(int n, int *result, ...) {
    if (result == NULL) return ERR_NULL;
    if (n < 3 || n > 5) return ERR_INVALID; // многоугольник - больше 3, ну и макс 5 пусть будет

    va_list args;
    va_start(args, result);

    double x[5], y[5]; // для хранения координат

    // читаем вершины
    for (int i = 0; i < n; i++) {
        x[i] = va_arg(args, double);
        y[i] = va_arg(args, double);
        
        if (isinf(x[i]) || isnan(x[i]) || isinf(y[i]) || isnan(y[i])) {
            va_end(args);
            return ERR_OVERFLOW;
        }
    }

    va_end(args); // завершаем чтение и дальше выполняем функцию

    // через векторное произведение для 2х векторов - определитель матрицы 2 на 2 = |a|*|b|*sin(phi)
    int sign = 0; // (длина всегда > 0, поэтому угол зависит от синуса)

    for (int i = 0; i < n; i++) { // по 3 точкам - чтобы был угол - 2 вектора
        int j = (i + 1) % n; // %n - по вершинам идем - замкнутость многоугольника
        int k = (i + 2) % n;

        // координаты 1 и 2 векторов
        double x1 = x[j] - x[i]; 
        double y1 = y[j] - y[i];
        double x2 = x[k] - x[j];
        double y2 = y[k] - y[j];

        double cross = x1*y2 - x2*y1;
        if (isinf(cross) || isnan(cross)) return ERR_OVERFLOW;

        if (cross != 0.0) {
            int curr_sign = (cross > 0.0) ? 1 : -1;  // 1 - синус положительный, -1 - синус отрицательный
            
            if (sign == 0) sign = curr_sign; // самый первый поворот
            else if (sign != curr_sign) {
                *result = 0; // вогнутый
                return SUCCESS;
            }
        }
    }

    *result = 1; // выпуклый
    return SUCCESS;
}


// значение многочлена в заданной точке
status func2(double x, int n, double *result, ...) {
    if (result == NULL) return ERR_NULL;
    if (n < 0 || n > 10) return ERR_INVALID;

    va_list args;
    va_start(args, result);

    double res = 0.0;
    
    for (int i = 0; i <= n; i++) {
        double coeff = va_arg(args, double);
        
        if (isinf(coeff) || isnan(coeff)) {
            va_end(args);
            return ERR_OVERFLOW;
        }

        res = res * x + coeff; // схема горнера

        if (isinf(res) || isnan(res)) {
            va_end(args);
            return ERR_OVERFLOW;
        }
    }

    va_end(args);
    *result = res;
    return SUCCESS;
}


// перевод в long long для чисел капрекара положительные
status str_to_ll(const char *str, int base, long long *out_val) {
    if (str == NULL || out_val == NULL) return ERR_NULL;
    if (base < 2 || base > 36) return ERR_INVALID;

    long long val = 0;
    int i = 0;

    while (str[i] == '0') i++;
    
    if (str[i] == '\0') {
        *out_val = 0;
        return SUCCESS;
    }

    for (; str[i] != '\0'; i++) {
        int digit = -1;
        
        if (str[i] >= '0' && str[i] <= '9') digit = str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'Z') digit = str[i] - 'A' + 10;
        else if (str[i] >= 'a' && str[i] <= 'z') digit = str[i] - 'a' + 10;
        else return ERR_INVALID;

        if (digit >= base) return ERR_INVALID;
        if (val > (LLONG_MAX - digit) / base) return ERR_OVERFLOW;

        val = val * base + digit; // схема горнера - перевод в 10сс
    }

    *out_val = val;
    return SUCCESS;
}

// ищет числа капрекара
status func3 (int base, int count, ...) {
    if (count <= 0) return ERR_INVALID;
    if (base < 2 || base > 36) return ERR_INVALID;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        char* num_str = va_arg(args, char*);
        if (num_str == NULL) {
            va_end(args);
            return ERR_NULL;
        }

        long long N;
        status st = str_to_ll(num_str, base, &N);
        if (st == ERR_INVALID) {
            printf("Число '%s' содержит недопустимые символы для системы счисления %d.\n", num_str, base);
            continue;
        }

        if (st == ERR_OVERFLOW) {
            va_end(args);
            return ERR_OVERFLOW;
        }

        if (N > 3000000000LL) {
            va_end(args);
            return ERR_OVERFLOW;
        }

        long long power = 1;
        long long temp = N;
        while (temp > 0) {
            if (power > LLONG_MAX / base) {
                va_end(args);
                return ERR_OVERFLOW;
            }
            power *= base; // вычисляет делитель base^k, где k - кол-во цифр в числе
            temp /= base; // считает количество цифр в определенной сс
        }

        long long N_sq = N * N; // сама суть - число в квадрате - 45^2 = 2025; 20+25 = 45
        long long R = N_sq % power; // правая часть числа в квадрате
        long long L = N_sq / power; // левая часть числа в квадрате

        if (R > 0 && (L + R == N))
            printf("Число '%s' является числом Капрекара.\n", num_str);
        else printf("Число '%s' не является числом Капрекара.\n", num_str);
    }

    va_end(args);
    return SUCCESS;
}


// среднее геометрическое
status func4(int count, double *result, ...) {
    if (result == NULL) return ERR_NULL;
    if (count <= 0 || count > 5) return ERR_INVALID;

    va_list args;
    va_start(args, result);
    
    double res = 1.0;
    for (int i = 0; i < count; i++) {
        double val = va_arg(args, double);
        
        if (val < 0.0) {
            va_end(args);
            return ERR_INVALID;
        }

        res *= val;
        if (isinf(res) || isnan(res)) {
            va_end(args);
            return ERR_OVERFLOW;
        }
    }

    va_end(args);
    *result = pow(res, 1.0 / count);
    if (isinf(*result) || isnan(*result)) return ERR_OVERFLOW;

    return SUCCESS;
}


// рекурсивное возведение в степень
status func5(const double x, const int power, double *res) {
    if (res == NULL) return ERR_NULL;
    if (power == 0) {
        *res = 1.0;
        return SUCCESS;
    }

    if (power < 0) {
        if (x == 0.0) return ERR_INVALID;
        if (power == INT_MIN) return ERR_OVERFLOW;

        double result;
        status st = func5(x, -power, &result); // считает нам положительную степень
        if (st == SUCCESS) {
            *res = 1.0 / result;
            if (isinf(*res) || isnan(*res)) return ERR_OVERFLOW;
        }
        return st;
    }

    double half_res;
    status st = func5(x, power/2, &half_res);
    if (st != SUCCESS) return st;

    if (power % 2 == 0) *res = half_res * half_res;
    else *res = x * half_res * half_res;

    if (isinf(*res) || isnan(*res)) return ERR_OVERFLOW;

    return SUCCESS;
}


// функции для проверки метода дихотомии
double eq1(double x) {
    return x * x - 4.0;
}

double eq2(double x) {
    return sin(x);
}

double eq3(double x) {
    return x * x * x - x - 2.0;
}

// метод дихотомии
status func6(double a, double b, const double eps, double (*func)(double), double *res) {
    if (res == NULL || func == NULL) return ERR_NULL;
    if (eps <= 0 || eps > 1.0) return ERR_INVALID;
    if (a >= b) return ERR_INVALID; // проверка что они разного знака

    double fa = func(a);
    double fb = func(b);

    if (isnan(fa) || isinf(fa) || isnan(fb) || isinf(fb)) return ERR_INVALID;
    if ((fa > 0 && fb > 0) || (fa < 0 && fb < 0) || (fa == 0.0 || fb == 0.0)) return ERR_INVALID;

    double mid;
    double fmid;

    while (fabs(b - a) >= eps) {
        mid = (a + b) / 2.0;
        fmid = func(mid);
        if (isnan(fmid) || isinf(fmid)) return ERR_INVALID;

        if (fmid == 0.0) {
            *res = mid;
            return SUCCESS;
        }

        if ((fa < 0 && fmid > 0) || (fa > 0 && fmid < 0)) b = mid;
        else {
            a = mid;
            fa = fmid;
        }
    }

    *res = (a + b) / 2.0;
    return SUCCESS;
}


void print_error(const status st) {
    switch (st) {
        case ERR_NULL: printf("Передан нулевой указатель (NULL).\n"); break;
        case ERR_INVALID: printf("Некорректные данные.\n"); break;
        case ERR_READ: printf("Не удалось прочитать данные.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение или недопустимое значение.\n"); break;
        default: printf("Неизвестная ошибка.\n"); break;
    }
}

void print_menu() {
    putchar('\n');
    printf(" 1. Проверка многоугольника на выпуклость\n");
    printf(" 2. Значение многочлена в заданной точке\n");
    printf(" 3. Поиск чисел Капрекара\n");
    printf(" 4. Среднее геометрическое\n");
    printf(" 5. Рекурсивное возведение в степень\n");
    printf(" 6. Метод дихотомии\n");
    printf(" 0. Выход из программы\n");
    putchar('\n');
    printf("Выберите действие (0-6): ");
}


status Overflow_INT(const char *str) {
    if (str == NULL) return ERR_NULL;

    char str_long_max[30];
    char str_long_min[30];
    snprintf(str_long_max, sizeof(str_long_max), "%d", INT_MAX);
    snprintf(str_long_min, sizeof(str_long_min), "%d", INT_MIN);

    int minus = 0;
    if (str[0] == '-'){
        minus = 1;
        str++;
    }
    else if (str[0] == '+') str++;
    while (str[0] == '0' && str[1] != '\0') str++;

    size_t len_str = strlen(str);
    size_t len_max = strlen(str_long_max);

    char *str_long_min2 = str_long_min + 1;
    size_t len_min = strlen(str_long_min2);

    int is_over = 0;

    if (minus){
        if (len_str > len_min) 
            is_over = 1;
        else if (len_str == len_min && strcmp(str, str_long_min2) > 0)
            is_over = 1;
    }
    else {
        if (len_str > len_max) 
            is_over = 1;
        else if (len_str == len_max && strcmp(str, str_long_max) > 0)
            is_over = 1;
    }

    if (is_over) return ERR_OVERFLOW;

    return SUCCESS;
}


status read_int(char *buffer, int *choice) {
    if (choice == NULL || buffer == NULL) return ERR_NULL;

    if (scanf("%49s", buffer) != 1) return ERR_READ;

    int c;
    while ((c = getchar()) != '\n' && c != EOF); // дочитываем до конца

    int start = (buffer[0] == '-' || buffer[0] == '+') ? 1 : 0;
    if (buffer[start] == '\0') return ERR_INVALID;

    for (int i = start; buffer[i] != '\0'; i++) {
        if (buffer[i] < '0' || buffer[i] > '9') return ERR_INVALID;
    }

    if (Overflow_INT(buffer) != SUCCESS) return ERR_OVERFLOW;

    *choice = atoi(buffer);
    return SUCCESS;
}


status read_double(char *buffer, double *val) {
    if (val == NULL || buffer == NULL) return ERR_NULL;

    if (scanf("%399s", buffer) != 1) return ERR_READ;

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    char *endptr;
    *val = strtod(buffer, &endptr);

    if (endptr == buffer) return ERR_INVALID;

    if (*endptr != '\0') return ERR_INVALID;

    if (isinf(*val) || isnan(*val)) return ERR_INVALID;

    return SUCCESS;
}
