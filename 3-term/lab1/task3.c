#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

int is_Flag(const char *str_flag){
    if (str_flag[0] != '-' && str_flag[0] != '/') return 0;
    if (!isalpha((unsigned char)str_flag[1])) return 0;
    if (str_flag[2] != '\0') return 0;
    
    return 1; // да
}

int Overflow(const char *str){
    char str_long_max[30];
    char str_long_min[30];
    snprintf(str_long_max, sizeof(str_long_max), "%ld", LONG_MAX);
    snprintf(str_long_min, sizeof(str_long_min), "%ld", LONG_MIN);

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

    if (is_over) return 1; // переполнилось

    return 0;
}

// квадратное уравнение
int solve_eq(const double eps, const double A, const double B, const double C){
    if (fabs(A) < eps){
        if (fabs(B) < eps){
            if (fabs(C) < eps){
                printf("Бесконечно много решений.\n");
                return -2;
            }
            else{
                printf("Нет решений.\n");
                return -1;
            }
        }
        else{
            printf("Один корень: x = %.8f\n", -C/B);
            return 1;
        }
    }

    else{
        double D = B*B - 4.0*A*C;
        if (isnan(D) || isinf(D)){
            printf("Переполнение при вычислении дискриминанта.\n");
            return 0;
        }

        if (D > eps){
            double x1 = (-B + sqrt(D)) / (2.0*A);
            double x2 = (-B - sqrt(D)) / (2.0*A);
            printf("Два корня: x1 = %.8f, x2 = %.8f\n", x1, x2);
            return 2;
        }

        else if(fabs(D) <= eps){
            double x = -B / (2.0 * A);
            printf("Один корень: x = %.8f\n", x);
            return 1;
        }

        else{
            printf("Нет действительных корней.\n");
            return 0;
        }    
    }  
}



int FlagQ(const double eps, const double a, const double b, const double c){
    double variants[6][3] = {
        {a, b, c}, {a, c, b}, {b, a, c}, 
        {b, c, a}, {c, a, b}, {c, b, a}
    };

    printf("Решения квадратного уравнения при всевозмоных уникальных подстановках:\n");
    int p = 1;
    for (int i = 0; i<6; i++){
        int duplic = 0;
        for (int j = 0; j<i; j++){
            if (fabs(variants[i][0] - variants[j][0]) < eps && 
                fabs(variants[i][1] - variants[j][1]) < eps &&
                fabs(variants[i][2] - variants[j][2]) < eps){
                duplic = 1;
                break;
            }
        }
        if (duplic) continue;
        
        printf("%d) ", p++);
        solve_eq(eps, variants[i][0], variants[i][1], variants[i][2]);
        putchar('\n');
    }

    return 1;
}


int FlagM(const long x, const long y){ // кратность
    if (x % y == 0) return 1; // да
    else return 0;
}


// прямоугольный треугольник
int FlagT(const double eps, const double a, const double b, const double c){
    if (a <= 0.0 || b <= 0.0 || c <= 0.0){
        printf("Стороны треугольника должны быть положительными.\n");
        return 0;
    }
    
    double max = a;
    double catet1 = 0.0, catet2 = 0.0;
    if (a > b - eps && a > c - eps){
        max = a;
        catet1 = b;
        catet2 = c;
    }
    else if (b > a - eps && b > c - eps){
        max = b;
        catet1 = a;
        catet2 = c;
    } 
    else {
        max = c;
        catet1 = a;
        catet2 = b;
    }

    double sum_sq = catet1*catet1 + catet2*catet2;
    double hyp_sq = max*max;
    if (isnan(sum_sq) || isnan(hyp_sq) || isinf(sum_sq) || isinf(hyp_sq)) {
        printf("Переполнение при вычислении квадратов сторон.\n");
        return 0;
    }

    if (fabs(sum_sq - hyp_sq) < eps) return 1; // да
    else return 0;
}


int main(int argc, char *argv[]){
    //проверка на корректность ввода
    if (argc < 2){
        printf("Недостаточно аргументов. Напишите %s <flag> <numbers>.\n", argv[0]);
        return 1;
    }

    if (!is_Flag(argv[1])){
        printf("Некорректный ввод. Напишите %s <flag> <numbers>.\n", argv[0]);
        return 1;
    }

    char flag = argv[1][1];
    if (flag != 'q' && flag != 'm' && flag != 't'){
        printf("Несуществующий флаг.\n");
        return 1;
    }

    if ((flag == 'q' || flag == 't') && argc != 6){
        printf("Некорректный ввод. Требуются эпсилон и 3 вещественных числа.\n");
        return 1;
    }

    else if(flag == 'm' && argc != 4){
        printf("Некорректный ввод. Требуются 2 ненулевых целых числа.\n");
        return 1;
    }

    // для m
    long x = 0, y = 0;
    if (flag == 'm'){
        for (int i = 2; i<=3; i++){
            int start_idx = 0;
            
            if (argv[i][0] == '-' || argv[i][0] == '+'){
                start_idx = 1;
            }

            if (argv[i][start_idx] == '\0'){
                printf("Неправильно введены числа.\n");
                return 1;
            }
            
            for (int j = start_idx; argv[i][j] != '\0'; j++){
                if (!isdigit((unsigned char)argv[i][j])){
                    printf("Неправильны введены чиcлa.\n");
                    return 1;
                }
            }
            
            // проверка переполнения для обоих чисел
            if (Overflow(argv[i])){
            printf("Переполнение.\n");
            return 1;
            }

            long value = atol(argv[i]);

            if (value == 0){
                printf("Числа должны быть ненулевыми.\n");
                return 1;
            }

            // сохраняем корректные значения
            if (i==2) x = value;
            else y = value;
        }
    }


    // для q и t
    double a = 0.0, b = 0.0, c = 0.0;
    double eps = 0.0;
    if (flag == 'q' || flag == 't'){

        if (*argv[2] == '\0'){
            printf("Некорректный ввод числа.\n");
            return 1;
        }

        char *endptr;
        eps = strtod(argv[2], &endptr);

        if (*endptr != '\0'){
            printf("Некорректный ввод числа.\n");
            return 1;
        }

        if (isinf(eps) || isnan(eps)){
            printf("Некорректный ввод эпсилон.\n");
            return 1;
        }

        if (eps <= 0.0 || eps > 1.0 ){
            printf("Некорректный ввод эпсилон. Оно должно быть в диапазоне (0, 1].\n");
            return 1;
        }

        for (int i = 3; i <= 5; i++){
            if (*argv[i] == '\0'){
                printf("Некорректный ввод чисел.\n");
                return 1;
            }

            double val = strtod(argv[i], &endptr);
        
            if (*endptr != '\0'){
                printf("Некорректный ввод чисел.\n");
                return 1;
            }

            if (isinf(val) || isnan(val)){
                printf("Некорректный ввод чисел.\n");
                return 1;
            }

            if (i==3) a = val;
            else if (i==4) b = val;
            else c = val; 
        }
    }

  
    switch(flag){
        case 'q':{
            FlagQ(eps, a, b, c);
            break;
        }

        case 'm':
            if(FlagM(x, y)){
                printf("%ld кратно %ld.\n", x, y);
            }
            else printf("%ld не кратно %ld.\n", x, y);
            break;
 
        case 't':
            if(FlagT(eps, a, b, c)){
                printf("%.8f, %.8f, %.8f являются длинами сторон прямоугольного треугольника.\n", a, b, c);
            }
            else printf("Не являются длинами сторон прямоугольного треугольника.\n");
            break;

        default:
            printf("Неизвестный флаг.\n");
            return 1;
    }

    return 0;
}