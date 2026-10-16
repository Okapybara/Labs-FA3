#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

typedef enum status{
    SUCCESS = 0, // успех
    ERR_NULL, // указатель нулевой
    ERR_INVALID, // наличие неверных значений
    ERR_READ, // ошибка чтения
    ERR_OVERFLOW, // переполнение
    ERR_INVALID_FLAG, // проверка флага
    ERR_INVALID_ARGC, // количество аргументов
    ERR_MEMORY // ошибка выделения памяти
} status;

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID_ARGC:
            printf("Неверное количество аргументов.\n");
            printf("Если вы хотите передать строку с пробелами используйте кавычки.\n");
            break;
        case ERR_INVALID_FLAG:
            printf("Неверный флаг. Есть только: -l, -r, -u, -n, или -c.\n");
            break;
        case ERR_INVALID:
            printf("Неверные данные.\n");
            break;
        case ERR_OVERFLOW:
            printf("Переполнение.\n");
            break;
        case ERR_NULL:
            printf("Нулевой указатель.\n");
            break;
        case ERR_MEMORY:
            printf("Ошибка выделения памяти.\n");
            break;
        default:
            printf("Неизвестная ошибка.\n");
            break;
    }
}


status unsigned_int(const char *str, unsigned int* seed) {
    if (str == NULL || seed == NULL) return ERR_NULL;
    if (str[0] == '\0') return ERR_INVALID;

    unsigned long long val = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return ERR_INVALID;
        
        val = val*10 + (unsigned long long)(str[i] - '0');
        if (val > UINT_MAX) return ERR_OVERFLOW;
    }
    
    *seed = (unsigned int)val;

    return SUCCESS;
}


status FlagL(const char *str, unsigned int *res) {
    if (str == NULL || res == NULL) return ERR_NULL;

    unsigned int len = 0;
    while (str[len] != '\0') {
        if (len == UINT_MAX) return ERR_OVERFLOW;
        len++;  
    }

    *res = len;
    return SUCCESS;
}


status FlagR(const char *str, char **res_str) {
    if (str == NULL || res_str == NULL) return ERR_NULL;
    
    unsigned int len = 0;
    while (str[len] != '\0') {
        if (len == UINT_MAX) return ERR_OVERFLOW;
        len++;
    }
    
    char *new_str = (char *)malloc((len + 1)*sizeof(char));
    if (new_str == NULL) return ERR_MEMORY;

    for (unsigned int i = 0; i < len; i++)
        new_str[i] = str[len-1-i];

    new_str[len] = '\0';

    *res_str = new_str;
    return SUCCESS;
}


status FlagU(const char *str, char **res_str) {
    if (str == NULL || res_str == NULL) return ERR_NULL;

    unsigned int len = 0;
    while (str[len] != '\0') {
        if (len == UINT_MAX) return ERR_OVERFLOW;
        len++;
    }

    char *new_str = (char *)malloc((len + 1)*sizeof(char));
    if (new_str == NULL) return ERR_MEMORY;

    for (unsigned int i = 0; i < len; i++){
        if (i % 2 != 0) new_str[i] = toupper((unsigned char)str[i]);
        else new_str[i] = str[i];
    }

    new_str[len] = '\0';

    *res_str = new_str;
    return SUCCESS;
}


status FlagN(const char *str, char **res_str) {
    if (str == NULL || res_str == NULL) return ERR_NULL;

    unsigned int len = 0;
    while (str[len] != '\0') {
        if (len == UINT_MAX) return ERR_OVERFLOW;
        len++;
    }

    char *new_str = (char *)malloc((len + 1)*sizeof(char));
    if (new_str == NULL) return ERR_MEMORY;

    unsigned int j = 0;
    for (unsigned int i = 0; i < len; i++) {
        if (isdigit((unsigned char)str[i])) {
            new_str[j] = str[i];
            j++;
        }
    }

    for (unsigned int i = 0; i < len; i++) {
        if (isalpha((unsigned char)str[i])) {
            new_str[j] = str[i];
            j++;
        }
    }

    for (unsigned int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)str[i]) && !isalpha((unsigned char)str[i])) {
            new_str[j] = str[i];
            j++;
        }
    }

    new_str[len] = '\0';

    *res_str = new_str;
    return SUCCESS;
}


status FlagC(char **strings, const unsigned int count, const unsigned int seed, char **res_str) {
    if (strings == NULL || res_str == NULL) return ERR_NULL;
    if (count == 0) return ERR_INVALID;
    srand(seed);

    unsigned int total_len = 0;
    for (unsigned int i = 0; i < count; i++) {
        if (strings[i] == NULL) return ERR_NULL;
        
        unsigned int len = 0;
        while (strings[i][len] != '\0') {
            if (len == UINT_MAX) return ERR_OVERFLOW;
            len++;
        }
        
        if (total_len > UINT_MAX - len) return ERR_OVERFLOW;
        total_len += len;
    }

    char *new_str = (char *)malloc((total_len + 1)*sizeof(char));
    if (new_str == NULL) return ERR_MEMORY;

    // копируем изначальный для перемешивания
    char** shuffled = (char**)malloc(count * sizeof(char*));
    if (shuffled == NULL) {
        free(new_str);
        return ERR_MEMORY;
    }

    for (unsigned int i = 0; i < count; i++)
        shuffled[i] = strings[i];

    // перемешиваем
    for (unsigned int i = count - 1; i > 0; i--) {
        unsigned int j = rand() % (i + 1); // рандом от нуля до количества
        char* temp = shuffled[i];
        shuffled[i] = shuffled[j];
        shuffled[j] = temp;
    }

    unsigned int pos = 0;
    for (unsigned int i = 0; i < count; i++) {
        unsigned int len = 0;
        while (shuffled[i][len] != '\0') {
            new_str[pos] = shuffled[i][len];
            pos++;
            len++;
        }
    }

    new_str[pos] = '\0';

    free(shuffled);

    *res_str = new_str;
    return SUCCESS;
}
