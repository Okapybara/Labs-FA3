#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

typedef enum status {
    SUCCESS = 0, // успех
    ERR_NULL, // указатель нулевой
    ERR_INVALID, // наличие неверных значений
    ERR_OVERFLOW, // переполнение
    ERR_NEGATIVE, // неотрицательность числа
    ERR_INVALID_ARGC, // количество аргументов
} status;


// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_NEGATIVE: printf("Число должно быть неотрицательным.\n"); break;
        case ERR_INVALID_ARGC: printf("Неверное количество аргументов.\n"); break;              
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

// перевод в число int
status validate_positive_int(const char *str, int *out_value) {
    if (str == NULL || out_value == NULL) return ERR_NULL;

    if (str[0] == '\0') return ERR_INVALID;
    if (str[0] == '-') return ERR_NEGATIVE;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return ERR_INVALID;
    }

    status res_over = Overflow_INT(str);
    if (res_over != SUCCESS) return res_over;

    *out_value = atoi(str);
    return SUCCESS;
}


// прибавление единицы
status plus_one(unsigned int *number) {
    if (number == NULL) return ERR_NULL;
    
    unsigned int mask = 1; // для рассморения каждого бита
    unsigned int carry = 1; // флаг переноса
    
    while (carry != 0 && mask != 0) {
        if ((*number & mask) != 0) { // если текущий бит == 1
            *number &= ~mask; //обнуляем текущий бит
            mask <<= 1; // сдвигаемся к следующему
        } 
        else {
            *number |= mask; // записываем в текущий бит 1
            carry = 0;
        }
    }
    
    if (mask == 0) return ERR_OVERFLOW;

    return SUCCESS;
}

// вычитание единицы
status minus_one(unsigned int *number) {
    if (number == NULL) return ERR_NULL;
    
    if (*number == 0) return ERR_OVERFLOW;
    
    unsigned int mask = 1;
    unsigned int borrow = 1;

    while (borrow != 0 && mask != 0) {
        if ((*number & mask) != 0) {
            *number &= ~mask;
            borrow = 0;
        } 
        else {
            *number |= mask;
            mask <<= 1;
        }
    }
    
    return SUCCESS;
}

// переворот строки
status reverse_string(char *buffer) {
    if (buffer == NULL) return ERR_NULL;

    unsigned int len = 0;
    while (buffer[len] != '\0') {
        if (plus_one(&len) != SUCCESS) return ERR_OVERFLOW;
    }

    if (len == 0) return SUCCESS;

    unsigned int start = 0;
    unsigned int end = len;
    
    if (minus_one(&end) != SUCCESS) return ERR_OVERFLOW;

    while (start < end) {
        char tmp = buffer[start];
        buffer[start] = buffer[end]; 
        buffer[end] = tmp;
        
        if (plus_one(&start) != SUCCESS) return ERR_OVERFLOW;
        if (minus_one(&end) != SUCCESS) return ERR_OVERFLOW;
    }

    return SUCCESS;
}


// основная функция
status convert_to_base_pow2(const unsigned int number, const int r, char *buffer) {
    if (buffer == NULL) return ERR_NULL;
    if (r < 1 || r > 5) return ERR_INVALID;

    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

    if (number == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return SUCCESS;
    }

    const unsigned int mask = ~(~0U << r);

    unsigned int index = 0;
    unsigned int temp = number;

    while (temp != 0) {
        buffer[index] = digits[temp & mask]; // temp & mask - остаток
        if (plus_one(&index) != SUCCESS) return ERR_OVERFLOW;
        temp >>= r; // сдвиг вправо - деление на 2^k
    }
    buffer[index] = '\0';

    status rev_status = reverse_string(buffer);
    if (rev_status != SUCCESS) return rev_status;

    return SUCCESS;
}
