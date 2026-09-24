#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>


int is_Flag(const char *str_flag){
    if (str_flag[0] != '-' && str_flag[0] != '/') return 0;
    if (!isalpha((unsigned char)str_flag[1])) return 0;
    if (str_flag[2] != '\0') return 0;
    
    return 1;
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

    if (is_over) return 1;

    return 0;
}


int FlagH(const long x, long *ptr_del_x, int *ptr_count){ // числа >0 и <=100 кратные х
    *ptr_count = 0;

    if (x == 0) return 1;

    for (long ch = 1; ch <= 100; ch++){
        if (ch % x == 0){ 
            ptr_del_x[*ptr_count] = ch;
            (*ptr_count)++;
        }
    }
    return 0;
}


int FlagP(const long x, int *res){ // число простое или составное
    
    if (x < 2){ // число ни простое ни составное
        *res = -1;
        return 0;
    }

    // i*i потому что после этого пары просто меняются местами
    // но так как у нас вдруг переполнение, то поделим оба на i
    for (long i = 2; i <= x/i; i++){ //составное
        if (x % i == 0){
            *res = 1;
            return 0;
        }
    }

    *res = 0; // если простое
    return 0;
}

int FlagS(long x, char *ptr_hex_digits, int *ptr_count){ // цифры в 16 сс
    *ptr_count = 0;
    const char hex_char[] = "0123456789ABCDEF";
    
    if (x == 0){
        ptr_hex_digits[0] = '0';
        *ptr_count = 1;
        return 0;
    }

    unsigned long modul_x;
    if (x < 0) {
        modul_x = (unsigned long)(-(x+1))+1;
    }
    else modul_x = x;

    while (modul_x > 0){
        ptr_hex_digits[*ptr_count] = hex_char[modul_x % 16];
        (*ptr_count)++;
        modul_x /= 16;
    }
    return 0;
}


int FlagE(long x, long table[10][10]){
    if (x <= 0 || x > 10) return 1;

    for (int base = 1; base <= 10; base++){
        long current = 1;
        for (int pow = 1; pow <= x; pow++){
            current *= base;
            table[base-1][pow-1] = current;
        }
    }

    return 0;
}


int FlagA(long x, long *ptr_summ){
    if (x <= 0) return 1;
    *ptr_summ = 0;

    for (long i = 1; i<=x; i++){
        if (LONG_MAX - *ptr_summ < i) return 2;
        *ptr_summ += i;
    }

    return 0;
}


int FlagF(long x, long *ptr_factor){
    *ptr_factor = 1;

    if (x < 0) return 1;

    for (long i = 1; i<=x; i++){
        if (LONG_MAX / *ptr_factor < i) return 2;
        *ptr_factor *= i;
    }

    return 0;
}


int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Некорректный ввод. Напишите %s <number> <flag> (или %s <flag> <number>).\n", argv[0], argv[0]);
        return 1;
    }

    // проверка кто флаг, а кто число
    int flag_idx = 0;
    int x_idx = 0;
    
    if (is_Flag(argv[1]) && !is_Flag(argv[2])){
        flag_idx = 1;
        x_idx = 2;
    }
    else if (is_Flag(argv[2]) && !is_Flag(argv[1])){
        flag_idx = 2;
        x_idx = 1;
    }
    else{
        printf("Некорректный ввод. Напишите %s <number> <flag> (или %s <flag> <number>).\n", argv[0], argv[0]);
        return 1;
    }

    // проверка что число, это число
    int start_idx = 0;
    if (argv[x_idx][0] == '-' || argv[x_idx][0] == '+'){
        start_idx = 1;
    }

    if (argv[x_idx][start_idx] == '\0'){
        printf("Неправильно введено число.\n");
        return 1;
    }
    
    for (int i = start_idx; argv[x_idx][i] != '\0'; i++){
        if (!isdigit((unsigned char)argv[x_idx][i])){
            printf("Неправильно введено число.\n");
            return 1;
        }
    }

    // проверка переполнения
    if (Overflow(argv[x_idx])){
        printf("Переполнение.\n");
        return 1;
    }

    long x = atol(argv[x_idx]);
    char flag_let = argv[flag_idx][1];
  
    switch(flag_let){
        case 'h':{
            long del_x[100];
            int count = 0;
            int resh = FlagH(x, del_x, &count);

            if (resh != 0){
                printf("Деление на ноль невозможно.\n");
                return 1;
            }
            else{
                if (count == 0) printf("Нет натуральных чисел до 100, кратных %ld.\n", x);
                else{
                    printf("Кратные числа:\n");
                    for(int i = 0; i<count; i++) printf("%ld ", del_x[i]);
                    putchar('\n');
                }
            }

            break;
        }

        case 'p':{
            int prost_sost = 10;
            FlagP(x, &prost_sost);

            if (prost_sost == 1) printf("Число %ld составное.\n", x);
            else if (prost_sost == 0) printf("Число %ld простое.\n", x);
            else printf("Число %ld ни простое ни составное.\n", x);

            break;
        }

        case 's':{
            char hex_digits[32];
            int count = 0;
            FlagS(x, hex_digits, &count);

            for(int i = count-1; i>=0; i--) printf("%c ", hex_digits[i]);
            putchar('\n');

            break;
        }
 
        case 'e':{
            long table[10][10];
            int rese = FlagE(x, table);

            if (rese != 0){
                printf("Число x для флага -e должно быть от 1 до 10.\n");
                return 1;
            }

            for (int base = 1; base <= 10; base++){
                for (int pow = 1; pow <= x; pow++){
                    printf("%-12ld", table[base-1][pow-1]);
                }
                putchar('\n');
            }

            break;
        }

        case 'a':{
            long summ = 0;
            int resa = FlagA(x, &summ);

            if (resa == 1){
                printf("Число должно быть натуральным.\n");
                return 1;
            }

            else if (resa == 2){
                printf("Переполнение суммы.\n");
                return 1;
            }

            else printf("%ld\n", summ);

            break;
        }

        case 'f':{
            long factor = 0;
            int resf = FlagF(x, &factor);

            if (resf == 1){
                printf("Число должно быть неотрицательным.\n");
                return 1;
            }

            else if (resf == 2){
                printf("Переполнение произведения.\n");
                return 1;
            }

            else printf("%ld\n", factor);

            break;
        }

        default:
            printf("Неизвестный флаг.\n");
            return 1;
    }

    return 0;
}
