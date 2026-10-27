#include "functions10.h"

int main() {
    double eps = 0.0;
    double a = 0.0;
    int degree = 0;
    char *input_str = NULL;
    int input_len = 0;
    double *g = NULL;

    double f_coeffs[MAX_DEGREE + 1]; 

    // ввод эпсилон
    while (1) {
        printf("Введите точность eps (вещественное число в диапазоне (0, 1]): ");
        if (read_string(&input_str, &input_len) != SUCCESS) return 1;
        
        if (input_len > 0 && read_double(input_str, &eps) == SUCCESS && eps > 0.0 && eps <= 1) {
            free(input_str);
            break;
        }
        printf("Неверная точность. Попробуйте снова.\n\n");
        free(input_str);
    }

    // ввод a
    while (1) {
        printf("Введите значение a (вещественное число): ");
        if (read_string(&input_str, &input_len) != SUCCESS) return 1;
        
        if (input_len > 0 && read_double(input_str, &a) == SUCCESS) {
            free(input_str);
            break;
        }
        printf("Неверное значение. Попробуйте снова.\n\n");
        free(input_str);
    }

    //ввод степени многочлена
    while (1) {
        printf("Введите степень многочлена (целое число от 0 до %d): ", MAX_DEGREE);
        if (read_string(&input_str, &input_len) != SUCCESS) return 1;
        
        if (input_len > 0 && read_int(input_str, &degree) == SUCCESS && degree >= 0 && degree <= MAX_DEGREE) {
            free(input_str);
            break;
        }
        printf("Неверная степень. Попробуйте снова.\n\n");
        free(input_str);
    }

    printf("\n");

    // ввод коэффициентов от младшего к старшему
    for (int i = 0; i <= degree; ++i) {
        while (1) {
            printf("Введите коэффициент f_%d (при x^%d): ", i, i);
            if (read_string(&input_str, &input_len) != SUCCESS) return 1;
            
            if (input_len > 0 && read_double(input_str, &f_coeffs[i]) == SUCCESS) {
                free(input_str);
                break;
            }
            printf("Неверное число. Попробуйте снова.\n");
            free(input_str);
        }
    }

    printf("Исходный многочлен f(x) = ");
    print_poly(f_coeffs, eps, degree, 0.0);

    status res = redecomposition(eps, a, &g, degree, f_coeffs[0], f_coeffs[1], f_coeffs[2], f_coeffs[3], f_coeffs[4],
                                 f_coeffs[5], f_coeffs[6], f_coeffs[7], f_coeffs[8], f_coeffs[9], f_coeffs[10]);

    if (res == SUCCESS) {
        printf("Переразложенный многочлен g(x) = ");
        print_poly(g, eps, degree, a);
    } 
    else {
        printf("\n");
        print_error(res);
    }

    free(g);
    return 0;
}
