#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <time.h> // для rand

typedef enum status{
    SUCCESS = 0, //успех
    ERR_NULL, // указатель нулевой
    ERR_INVALID, // наличие неверных значений
    ERR_READ, // ошибка чтения
    ERR_OVERFLOW // переполнение
} status;

#define STATIC_SIZE 10

// переполнение
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



// функция для статического массива
status static_array(int *arr, const int size, int *res_min, int *res_max) {
    if (arr == NULL || res_min == NULL || res_max == NULL) return ERR_NULL;
    if (size <= 0) return ERR_INVALID;

    int min_idx = 0;
    int max_idx = 0;

    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[min_idx]) min_idx = i;
        if (arr[i] > arr[max_idx]) max_idx = i;
    }

    *res_min = arr[min_idx];
    *res_max = arr[max_idx];

    int temp = arr[min_idx];
    arr[min_idx] = arr[max_idx];
    arr[max_idx] = temp;

    return SUCCESS;
}

