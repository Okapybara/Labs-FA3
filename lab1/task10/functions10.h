#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

typedef enum status{
    SUCCESS = 0, //успех
    ERR_NULL, // указатель нулевой
    ERR_INVALID, // наличие неверных значений
    ERR_READ, // ошибка чтения
    ERR_INVALID_BASE, // неверная система счисления
    ERR_OVERFLOW // переполнение
} status;


status Overflow(const char *str){
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


// ввод числа
status read_number (char *number, int *isstop) {
    if (number == NULL || isstop == NULL) return ERR_NULL;

    *isstop = 0;

    if (scanf("%19s", number) != 1) return ERR_READ;
    
    int next = getchar(); // проверка того что считалось все число
                          // если нет и след символ это не разделитель, то переполнение
    if (next != ' ' && next != '\n' && next != '\t' && next != '\r' && next != EOF)
        return ERR_OVERFLOW;
    
    if (strcmp(number, "Stop") == 0 || strcmp(number, "stop") == 0) *isstop = 1;

    return SUCCESS;
}


// проверка на лишние символы
status correct_number(char *number, int base) {
    if (number == NULL) return ERR_NULL;
    if (base < 2 || base > 36) return ERR_INVALID_BASE;

    for (int i = 0; number[i] != '\0'; i++) {
        char num = number[i];
        int value = 0;

        if (num >= '0' && num <= '9') value = num - '0';
        else if (num >= 'A' && num <= 'Z') value = num - 'A' + 10; // тк а=10 и тд, поэтому +10
        else return ERR_INVALID;

        if (value >= base) return ERR_INVALID;
    }
    return SUCCESS;
}


// удаляет ведущие нули
status delete_leading_zeros(char *number) {
    if (number == NULL) return ERR_NULL;
    
    int null = 0;
    while (number[null] == '0') null++;

    if (number[null] == '\0') {
        number[0] = '0';
        number[1] = '\0';
    }

    else {
        int i = 0;
        while (number[null] != '\0') {
            number[i] = number[null];
            i++;
            null++;
        }
        number[i] = '\0';
    }

    return SUCCESS;
}


// перевод в 10 сс
status to_base_ten (const char *str, int base, unsigned long long *res) {
    if (str == NULL || res == NULL) return ERR_NULL;
    if (base < 2 || base > 36) return ERR_INVALID_BASE;

    *res = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        int value = 0;

        if (ch >= '0' && ch <= '9') value = ch - '0';
        else if (ch >= 'A' && ch <= 'Z') value = ch - 'A' + 10;

        if (*res > (ULLONG_MAX - value) / base) {
            return ERR_OVERFLOW;
        }

        *res = (*res * base) + value; // схема Горнера
    }

    return SUCCESS;
}


// перевод в другую сс
status to_other_base(unsigned long long value, char *str_res, int base, int max_len){
    if (str_res == NULL || max_len <= 0) return ERR_NULL;
    if (base < 2 || base > 36) return ERR_INVALID_BASE;

    if (value == 0) {
        str_res[0] = '0';
        str_res[1] = '\0';
        return SUCCESS;
    }

    char temp[65];
    int index = 0;

    while (value > 0) {
        if (index > 64) return ERR_OVERFLOW;
        int num = value % base;

        if (num < 10) temp[index] = '0' + num;
        else temp[index] = 'A' + (num - 10); // тк от 10 идут буквы, а мы к а прибавляем чтобы получить букву

        index++;
        value = value / base;
    }
    temp[index] = '\0'; // конец числа

    if (index + 1 > max_len) // проверка что все влезет в итоговый массив и +1 на \0
        return ERR_OVERFLOW;

    int pos = 0;
    for (int i = index-1; i >= 0; i--){ // переворачиваем
        str_res[pos] = temp[i];
        pos++;
    }

    str_res[pos] = '\0';
    
    return SUCCESS;   
}
