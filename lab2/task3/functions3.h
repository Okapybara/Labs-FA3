#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h> 

typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID_ARGC, // неверное количество аргументов
    ERR_INVALID, // наличие неверных значений
    ERR_NULL, // указатель нулевой
    ERR_OVERFLOW, // переполнение
    ERR_MEMORY, // ошибка выделения памяти
    ERR_READ, // ошибка чтения
    ERR_INVALID_NAME, // неверный ввод имени файла
    ERR_FILE_COUNT, // неверное количество файлов
    ERR_EMPTY_STR, // пустая строка для поиска
    ERR_NOT_TEXT // не текстовый файл
} status;

#define MAX_FILES 5
#define MAX_PATH_LEN 4096
#define INITIAL_BUF_SIZE 1024

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID_ARGC: printf("Неверное количество аргументов.\n"); break;
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_MEMORY: printf("Ошибка выделения памяти.\n"); break;            
        case ERR_READ: printf("Ошибка чтения файла.\n"); break;
        case ERR_INVALID_NAME: printf("Неверный ввод имени файла.\n"); break;
        case ERR_FILE_COUNT: printf("Количество файлов должно быть от 1 до %d.\n", MAX_FILES); break;
        case ERR_EMPTY_STR: printf("Искомая строка пустая.\n"); break; 
        case ERR_NOT_TEXT: printf("Файл не является текстовым.\n"); break;
        default: printf("Неизвестная ошибка.\n"); break;
    }
}


// проверяет расширение файлов
status text_extension(const char *file) {
    if (file == NULL) return ERR_NULL;

    const char *dot = strrchr(file, '.'); // последнее вхождение точки
    
    if (dot == NULL) return ERR_NOT_TEXT; // остаток строки от dot включительно
    if (strcmp(dot, ".txt") != 0 && strcmp(dot, ".TXT") != 0) return ERR_NOT_TEXT;
    
    return SUCCESS;
}


// переполнение 
int Overflow_INT(const char *str){
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


// чтение подстроки
status read_substring(char **out_str, int *out_len) {
    if (out_str == NULL || out_len == NULL) return ERR_NULL;

    int capacity = INITIAL_BUF_SIZE;
    int length = 0;
    char *buffer = (char *)malloc(capacity * sizeof(char));
    
    if (buffer == NULL) return ERR_MEMORY;

    printf("Введите подстроку для поиска (завершите ввод Ctrl+D):\n");

    int ch;
    while ((ch = getchar()) != EOF) {
        if (length >= capacity - 1) {
            if (capacity > INT_MAX / 2) { // проверка на переполнение
                free(buffer);
                return ERR_MEMORY;
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
    
    if (length == 0) {
        free(buffer);
        return ERR_EMPTY_STR;
    }

    *out_str = buffer;
    *out_len = length;
    return SUCCESS;
}


// еще не конец
status search_in_files(const char *substr, int substr_len, int file_count, ...) {

}
