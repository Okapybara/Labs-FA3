#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

typedef enum status{
    SUCCESS = 0, //успех
    ERR_READ, // ошибка чтения
    ERR_WRITE, // ошибка записи
    ERR_NULL, // указатель нулевой
    ERR_INVALID_CHAR, // наличие неверных значений в файле
    ERR_INVALID_NAME, // неверный ввод имени файла
    ERR_INVALID_BASE, // неверная система счисления
    ERR_OVERFLOW, // переполнение
    EMPTY_FILE, // пустой файл (только из пробелов и тд)
    SAME_FILES, // одинаковые имена файлов
    NOT_TEXT, // не текстовый файл
} status;

#define MAX_WORD_LEN 4000


// сравнивает имена файлов
status same_files(const char *file1, const char *file2 ) {
    if (file1 == NULL || file2 == NULL) return ERR_NULL;

    if (strchr(file1, '\\') != NULL || strchr(file2, '\\') != NULL) return ERR_INVALID_NAME;

    const char *file1_last_slash = strrchr(file1, '/');
    const char *file2_last_slash = strrchr(file2, '/');

    const char *file1_filename = file1;
    const char *file2_filename = file2;

    if (file1_last_slash != NULL) file1_filename = file1_last_slash + 1;
    if (file2_last_slash != NULL) file2_filename = file2_last_slash + 1;

    if (strlen(file1_filename) == 0 || strlen(file2_filename) == 0) return ERR_INVALID_NAME;
    
    if (strcmp(file1_filename, file2_filename) == 0) return SAME_FILES;
    else return SUCCESS;
}


// проверяет расширение файлов
status text_extension(const char *file) {
    if (file == NULL) return ERR_NULL;

    const char *dot = strrchr(file, '.'); // последнее вхождение точки
    
    if (dot == NULL) return NOT_TEXT; // остаток строки от dot включительно
    if (strcmp(dot, ".txt") != 0 && strcmp(dot, ".TXT") != 0) return NOT_TEXT;
    
    return SUCCESS;
}

// проверяет наличие лишних символов во входном файле
status extra_symbols_file(FILE *fin) {
    if (fin == NULL) return ERR_NULL;

    int ch;
    while ((ch = fgetc(fin)) != EOF) {
        if (ferror(fin)) return ERR_READ;
        if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || 
            (ch >= 'A' && ch <= 'Z') || (ch == ' ') ||
            (ch == '\t') || (ch == '\n') || (ch == '\r'))
            continue;
        else return ERR_INVALID_CHAR;
    }
    return SUCCESS;
}

// вытаскивает и собирает число
status read_number(FILE *fin, char *str_res) {
    if (fin == NULL || str_res == NULL) return ERR_NULL;

    int ch;
    int pos = 0;
    int too_long = 0;

    int first_ch = EOF;
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return ERR_READ;
        if (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r') {
            first_ch = ch;
            break;
        }
    }
    if (first_ch == EOF) return EMPTY_FILE;

    str_res[pos] = (char)first_ch;
    pos++;

    while ((ch = fgetc(fin)) != EOF && (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r')) {
        if (ferror(fin)) return ERR_READ;
        if (pos < MAX_WORD_LEN-1){
            str_res[pos] = ch;
            pos++;
        }
        else too_long = 1;
    }
    str_res[pos] = '\0';

    if (too_long) return ERR_OVERFLOW;
    return SUCCESS;   
}


// удаляются ведущие нули
status delete_leading_zeros(char *str) {
    if (str == NULL) return ERR_NULL;
    
    int null = 0;
    while (str[null] == '0') null++;

    if (str[null] == '\0') {
        str[0] = '0';
        str[1] = '\0';
    }

    else {
        int i = 0;
        while (str[null] != '\0') {
            str[i] = str[null];
            i++;
            null++;
        }
        str[i] = '\0';
    }

    return SUCCESS;
}


// строковый формат букв
status to_lower(char *str) {
    if (str == NULL) return ERR_NULL;

    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower(str[i]);
    }

    return SUCCESS;
}


// минимальная система счисления
status min_base (const char *str, int *res_base) {
    if (str == NULL || res_base == NULL) return ERR_NULL;

    int max_digit = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        int value = 0;

        if (ch >= '0' && ch <= '9') value = ch - '0';
        else if (ch >= 'a' && ch <= 'z') value = ch - 'a' + 10; // +10 это после 9 же, 'а' = 97

        if (value > max_digit) max_digit = value;
    }

    int base = max_digit + 1;
    if (base < 2) base = 2;

    *res_base = base;

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
        else if (ch >= 'a' && ch <= 'z') value = ch - 'a' + 10;

        if (*res > (ULLONG_MAX - value) / base) {
            return ERR_OVERFLOW;
        }

        *res = (*res * base) + value; // схема Горнера
    }

    return SUCCESS;
}
