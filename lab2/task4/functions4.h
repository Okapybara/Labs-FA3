#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID, // наличие неверных значений
    ERR_NULL, // указатель нулевой
    ERR_OVERFLOW, // переполнение
    ERR_MEMORY, // ошибка выделения памяти
} status;

#define INITIAL_BUF_SIZE 1024
#define MAX_FILES 1000
#define MAX_CAPACITY 10000

// структура для хранения файла
typedef struct File {
    char *name;
    int size;
    int importance;
} File;

// структура для результата
typedef struct Result {
    char **selected_files; // массив выбранных файлов 
    int count; // количество выбранных файлов
    int total_size; // суммарный размер
    int total_importance; // суммарная важность
} Result;


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


// проверка уникальности имени файла
int is_name_unique(const char *name, File *files, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            return 0;
        }
    }
    return 1;
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


status algorithm(File *files, int n, int capacity, Result *result) {
    if (files == NULL || result == NULL) return ERR_NULL;

    result->selected_files = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;

    // таблица - заполняем важностью - ищем макс важность
    int **table = (int **)malloc((n + 1) * sizeof(int *));
    if (table == NULL) return ERR_MEMORY;

    for (int i = 0; i <= n; i++) {
        table[i] = (int *)calloc(capacity + 1, sizeof(int));  // сразу заполняет нулями
        if (table[i] == NULL) {
            for (int j = 0; j < i; j++) free(table[j]);
            free(table);
            return ERR_MEMORY;
        }
    }

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (files[i - 1].size <= w) {
                int skip = table[i - 1][w]; // верх
                int take = table[i - 1][w - files[i - 1].size] + files[i - 1].importance;
                table[i][w] = (take > skip) ? take : skip;
            } 
            else table[i][w] = table[i - 1][w]; // верх
        }
    }

    result->total_importance = table[n][capacity];

    // ищем взятые файлы    
    int w = capacity;
    int count = 0;
    for (int i = n; i >= 1; i--) {
        if (table[i][w] != table[i - 1][w]) {
            count++;
            w -= files[i - 1].size;
        }
    }
    result->count = count;

    if (count > 0) {
        result->selected_files = (char **)malloc(count * sizeof(char *)); // под имена файлов
        if (result->selected_files == NULL) {
            for (int i = 0; i <= n; i++) free(table[i]);
            free(table);
            return ERR_MEMORY;
        }

        w = capacity;
        int idx = count - 1; // с конца собираем

        for (int i = n; i >= 1; i--) {
            if (table[i][w] != table[i - 1][w]) {
                result->selected_files[idx] = (char *)malloc((strlen(files[i - 1].name) + 1) * sizeof(char));
                if (result->selected_files[idx] == NULL) {
                    for (int j = idx + 1; j < count; j++) free(result->selected_files[j]);
                    free(result->selected_files);
                    result->selected_files = NULL;
                    result->count = 0;
                    for (int k = 0; k <= n; k++) free(table[k]);
                    free(table);
                    return ERR_MEMORY;
                }
                
                strcpy(result->selected_files[idx], files[i - 1].name);
                result->total_size += files[i - 1].size;
                w -= files[i - 1].size;
                idx--;
            }
        }
    }

    for (int i = 0; i <= n; i++) free(table[i]);
    free(table);

    return SUCCESS;
}
