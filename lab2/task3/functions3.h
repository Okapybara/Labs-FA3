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

// структуры для хранения результатов
typedef struct Match {
    int line; // номер строки (с 1)
    int col;  // номер символа (с 1)
} Match;

typedef struct MatchList {
    Match *data;     // динамический массив структур - номер строки и символа всех вхождений
    int count;       // кол-во найденных
    int capacity;    // вместимость
} MatchList;

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
status read_substring(char **str, int *len) {
    if (str == NULL || len == NULL) return ERR_NULL;

    int capacity = INITIAL_BUF_SIZE;
    int length = 0;
    char *buffer = (char *)malloc(capacity * sizeof(char));
    
    if (buffer == NULL) return ERR_MEMORY;

    printf("Введите подстроку для поиска (завершите ввод Ctrl+D):\n");

    int ch;
    while ((ch = getchar()) != EOF) {
        if (ch == '\r') continue;
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

    *str = buffer;
    *len = length;
    return SUCCESS;
}


// чтение файла в динамический массив
status read_file(const char *filename, char **buf, int *len) {
    if (filename == NULL || buf == NULL || len == NULL) return ERR_NULL;

    FILE *file = fopen(filename, "r");
    if (file == NULL) return ERR_READ;

    int capacity = INITIAL_BUF_SIZE;
    int length = 0;
    char *buffer = (char *)malloc(capacity * sizeof(char));
    if (buffer == NULL) {
        fclose(file);
        return ERR_MEMORY;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\r') continue;
        if (length >= capacity - 1) {
            if (capacity > INT_MAX / 2) {
                free(buffer);
                fclose(file);
                return ERR_MEMORY;
            }
            capacity *= 2;
            char *new_buf = (char *)realloc(buffer, capacity * sizeof(char));
            if (new_buf == NULL) {
                free(buffer);
                fclose(file);
                return ERR_MEMORY;
            }
            buffer = new_buf;
        }
        buffer[length++] = (char)ch;
    }
    
    buffer[length] = '\0';
    fclose(file);

    *buf = buffer;
    *len = length;
    return SUCCESS;
}


// поиск в 1 файле
status file_search(const char *filename, const char *substr, int substr_len, MatchList *results) {
    if (filename == NULL || substr == NULL || substr_len <= 0 || results == NULL) {
        return ERR_NULL;
    }

    // структура результатов
    results->capacity = 16;
    results->count = 0;
    results->data = (Match *)malloc(results->capacity * sizeof(Match));
    if (results->data == NULL) return ERR_MEMORY;

    // чтение файла
    char *file_buf = NULL;
    int file_len = 0;
    status read_status = read_file(filename, &file_buf, &file_len);
    if (read_status != SUCCESS) {
        free(results->data);
        results->data = NULL;
        return read_status;
    }

    // поиск подстроки - сравниваем подстроку посимвольно слева направо
    int current_line = 1;
    int current_col = 1;

    for (int i = 0; i <= file_len - substr_len; i++) { // вычитание - находит последнюю возможную позицию для начала подстроки
        int is_match = 1;
        for (int k = 0; k < substr_len; k++) {
            if ((unsigned char)file_buf[i+k] != (unsigned char)substr[k]) {
                is_match = 0;
                break;
            }
        }

        // сохраняем, если нашли
        if (is_match) {
            if (results->count >= results->capacity) {
                if (results->capacity > INT_MAX / 2) break;
                results->capacity *= 2;
                Match *new_data = (Match *)realloc(results->data, results->capacity * sizeof(Match));
                if (new_data == NULL) {
                    free(results->data);
                    free(file_buf);
                    results->data = NULL;
                    return ERR_MEMORY;
                }
                results->data = new_data;
            }
            
            results->data[results->count].line = current_line;
            results->data[results->count].col = current_col;
            results->count++;
        }

        if ((unsigned char)file_buf[i] == '\n') {  // переход на след строку
            current_line++;
            current_col = 1;
        }
        else { // иначе просто двигаемся вправо
            unsigned char byte = (unsigned char)file_buf[i];
            if (byte < 0x80 || byte >= 0xC0) current_col++; // для верного подсчета русских букв
        }
    }

    free(file_buf);
    return SUCCESS;
}


// вывод найденного
status print_search_res(const char *filename, const MatchList *results) {
    if (filename == NULL) return ERR_NULL;

    putchar('\n');
    printf("Файл: \"%s\"\n", filename);
    
    if (results->count == 0) printf("Вхождений не найдено.\n");
    else {
        printf("Найдено вхождений: %d\n", results->count);
        for (int i = 0; i < results->count; ++i) {
            printf("Строка: %d, Символ: %d\n", results->data[i].line, results->data[i].col);
        }
    }

    return SUCCESS;
}


// функция задания с переменным числом аргументов
status search_in_files(const char *substr, int substr_len, int file_count, ...) {
    if (substr == NULL || substr_len <= 0 || file_count <= 0) return ERR_NULL;

    va_list args;
    va_start(args, file_count);

    status res = SUCCESS;

    for (int i = 0; i < file_count; ++i) {
        char *filename = va_arg(args, char *);
        
        if (filename == NULL) continue;

        MatchList results = {NULL, 0, 0};
        status search = file_search(filename, substr, substr_len, &results);

        if (search == ERR_READ) {
            putchar('\n');
            printf("Не удалось открыть или прочитать файл: \"%s\"\n", filename);
            res = ERR_READ;
        }
        else if (search == ERR_MEMORY) {
            putchar('\n');
            printf("Не удалось выделить память при обработке файла: \"%s\"\n", filename);
            res = ERR_MEMORY;
        }
        else print_search_res(filename, &results);

        if (results.data != NULL) free(results.data);
    }

    va_end(args);
    return res;
}
