#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID_ARGC, // неверное количество аргументов
    ERR_INVALID, // наличие неверных значений
    ERR_NULL, // указатель нулевой
    ERR_OVERFLOW, // переполнение
    ERR_MEMORY, // ошибка выделения памяти
    ERR_READ, // ошибка чтения
    ERR_INVALID_NAME, // неверный ввод имени файла
    ERR_NOT_TEXT, // не текстовый файл
    ERR_EOF, // конец файла
    SAME_FILES, // одинаковый входной и выходной файл
    ERR_NOT_FOUND // для поиска
} status;

typedef struct Student {
    unsigned int id;
    char *name;
    char *surname;
    char *group;
    unsigned char *marks;
} Student;

#define EPSILON 1e-9 // для среднего балла
#define MAX_STUDENTS 100000

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
        case ERR_NOT_TEXT: printf("Файл не является текстовым.\n"); break;
        case SAME_FILES: printf("Файлы совпадают.\n"); break;
        case ERR_NOT_FOUND: printf("Студент не найден.\n"); break;
        case ERR_EOF: break;
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

// переполнение 
status Overflow_UINT(const char *str){
    if (str == NULL) return ERR_NULL;

    char str_int_max[20];
    snprintf(str_int_max, sizeof(str_int_max), "%u", UINT_MAX);

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

// перевод в целый тип >= 0
status read_uint(const char *str, const char **endptr, unsigned int *out_val) {
    if (str == NULL || endptr == NULL || out_val == NULL) return ERR_NULL;

    const char *p = str;
    while (isspace((unsigned char)*p)) p++;

    if (*p == '\0') return ERR_INVALID;
    
    const char *num_start = p;
    while (isdigit((unsigned char)*p)) p++;

    if (p == num_start) return ERR_INVALID;

    if (*p != '\0' && !isspace((unsigned char)*p)) return ERR_INVALID;

    size_t len = p - num_start;
    if (len > 15) return ERR_OVERFLOW;

    char temp[20];
    strncpy(temp, num_start, len);
    temp[len] = '\0';

    status overflow = Overflow_UINT(temp);
    if (overflow != SUCCESS) return overflow;

    unsigned int val = (unsigned int)atoi(temp);

    *out_val = val;
    *endptr = p;
    return SUCCESS;
}

// проверяет слово имя/фамилию
status validate_word(const char *str, const char **endptr, char **word) {
    if (str == NULL || endptr == NULL || word == NULL) return ERR_NULL;

    const char *p = str;
    size_t len = 0;

    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return ERR_INVALID;

    const char *start = p;

    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            len++;
            p++;
        }
        else break;
    }

    if (len == 0) return ERR_INVALID;
    if (len > 100) return ERR_OVERFLOW;

    *word = (char *)malloc((len + 1) * sizeof(char));
    if (*word == NULL) return ERR_MEMORY;

    for (int i = 0; i < len; i++) 
        (*word)[i] = start[i];

    (*word)[len] = '\0';

    *endptr = p;
    return SUCCESS;
}

// проверяет группу
status validate_group(const char *str, const char **endptr, char **word) {
    if (str == NULL || endptr == NULL || word == NULL) return ERR_NULL;

    const char *p = str;
    size_t len = 0;

    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return ERR_INVALID;

    const char *start = p;

    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        if ((isalnum(c) || c == '-' || c == '_')) {
            len++;
            p++;
        }
        else break;
    }

    if (len == 0) return ERR_INVALID;
    if (len > 100) return ERR_OVERFLOW;

    *word = (char *)malloc((len + 1) * sizeof(char));
    if (*word == NULL) return ERR_MEMORY;

    for (int i = 0; i < len; i++) 
        (*word)[i] = start[i];

    (*word)[len] = '\0';

    *endptr = p;
    return SUCCESS;
}

// чтение оценки
status read_mark(const char *str, const char **endptr, unsigned char *out_val) {
    if (str == NULL || endptr == NULL || out_val == NULL) return ERR_NULL;

    const char *p = str;
    while (isspace((unsigned char)*p)) p++;

    if (*p == '\0') return ERR_INVALID;
    
    const char *num_start = p;
    int digit_count = 0;
    while (isdigit((unsigned char)*p)) {
        digit_count++;
        if (digit_count > 3) return ERR_OVERFLOW;
        p++;
    }

    if (digit_count == 0) return ERR_INVALID;

    if (*p != '\0' && !isspace((unsigned char)*p)) return ERR_INVALID;

    size_t len = p - num_start;

    char temp[10];
    strncpy(temp, num_start, len);
    temp[len] = '\0';

    unsigned int val = (unsigned int)atoi(temp);
    if (val > 255) return ERR_OVERFLOW;

    *out_val = (unsigned char)val;
    *endptr = p;
    return SUCCESS;
}

// освобождение студента одного
void free_student(Student *student) {
    if (student == NULL) return;
    free(student->name);
    free(student->surname);
    free(student->group);
    free(student->marks);

    student->name = NULL;
    student->surname = NULL;
    student->group = NULL;
    student->marks = NULL;
}

// чтение строки из файла
status read_line_dynamic(FILE *file, char **out_line) {
    if (file == NULL || out_line == NULL) return ERR_NULL;

    size_t capacity = 64;
    size_t len = 0;
    
    char *buffer = (char *)malloc(capacity * sizeof(char));
    if (buffer == NULL) return ERR_MEMORY;

    int ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        if (len >= capacity - 1) {
            if (capacity > INT_MAX / 2) {
                free(buffer);
                return ERR_MEMORY;
            }
            capacity *= 2;
            char *temp = (char *)realloc(buffer, capacity * sizeof(char));
            if (temp == NULL) {
                free(buffer);
                return ERR_MEMORY;
            }
            buffer = temp;
        }
        buffer[len++] = (char)ch;
    }
    if (len == 0 && ch == EOF) {
        free(buffer);
        *out_line = NULL;
        return ERR_READ;
    }

    if (len > 0 && buffer[len - 1] == '\r') buffer[len - 1] = '\0';
    else buffer[len] = '\0';

    *out_line = buffer;
    return SUCCESS;
}

// чтение строки студента 1 структура
status student_line(const char *line, Student *out_student) {
    if (line == NULL || out_student == NULL) return ERR_NULL;

    out_student->id = 0;
    out_student->name = NULL;
    out_student->surname = NULL;
    out_student->group = NULL;
    out_student->marks = NULL;

    const char *p = line;
    status st;

    st = read_uint(p, &p, &out_student->id);
    if (st != SUCCESS) { 
        free_student(out_student); 
        return st;
    }

    st = validate_word(p, &p, &out_student->name);
    if (st != SUCCESS) { 
        free_student(out_student);
        return st;
    }

    st = validate_word(p, &p, &out_student->surname);
    if (st != SUCCESS) {
        free_student(out_student);
        return st;
    }

    st = validate_group(p, &p, &out_student->group);
    if (st != SUCCESS) {
        free_student(out_student); 
        return st; 
    }

    out_student->marks = (unsigned char *)malloc(5 * sizeof(unsigned char));
    if (out_student->marks == NULL) { 
        free_student(out_student); 
        return ERR_MEMORY; 
    }

    for (int i = 0; i < 5; i++) {
        unsigned char mark;
        st = read_mark(p, &p, &mark);
        if (st != SUCCESS) { 
            free_student(out_student); 
            return st; 
        }
        out_student->marks[i] = mark;
    }

    while (isspace((unsigned char)*p)) p++;
    if (*p != '\0') {
        free_student(out_student);
        return ERR_INVALID;
    }

    return SUCCESS;
}

// освобождение массива студентов
void free_students(Student *array, int count) {
    if (array == NULL) return;
    for (int i = 0; i < count; i++) {
        free_student(&array[i]);
    }
    free(array);
}

// чтение студентов из файла в динамический массив
status read_students(FILE *fin, Student **out_array, int *out_count) {
    if (fin == NULL || out_array == NULL || out_count == NULL) return ERR_NULL;

    *out_array = NULL;
    *out_count = 0;

    int capacity = 10;
    Student *array = (Student *)malloc(capacity * sizeof(Student));
    if (array == NULL) return ERR_MEMORY;

    int count = 0;
    char *line = NULL;

    while (1) {
        status st = read_line_dynamic(fin, &line);
        if (st == ERR_READ) break; // конец файла
        
        if (st != SUCCESS) {
            free_students(array, count);
            return st;
        }

        if (line[0] == '\0') { // пустые строки пропускаем
            free(line);
            continue;
        }

        Student temp_student; // читаем студента одного
        st = student_line(line, &temp_student);
        free(line);

        if (st != SUCCESS) {
            free_students(array, count);
            return st;
        }

        if (count == capacity) {
            if (capacity > INT_MAX / 2) {
                free_student(&temp_student);
                free_students(array, count);
                return ERR_MEMORY;
            }
            capacity *= 2;
            Student *temp_realloc = (Student *)realloc(array, capacity * sizeof(Student));
            if (temp_realloc == NULL) {
                free_student(&temp_student);
                free_students(array, count);
                return ERR_MEMORY;
            }
            array = temp_realloc;
        }

        array[count] = temp_student;
        count++;
        if (count >= MAX_STUDENTS) {
            free_students(array, count);
            return ERR_OVERFLOW;
        }
    }

    *out_array = array;
    *out_count = count;
    
    return SUCCESS;
}

// для ввода
status read_num(unsigned int *num) {
    if (num == NULL) return ERR_NULL;
    char buffer[64];
    
    int ret = scanf("%63s", buffer);
    
    if (ret == EOF) return ERR_EOF;
    if (ret != 1) return ERR_INVALID;

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if (buffer[0] == '\0') return ERR_INVALID;

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] < '0' || buffer[i] > '9') {
            return ERR_INVALID;
        }
    }

    if (Overflow_UINT(buffer) != SUCCESS) return ERR_OVERFLOW;

    *num = (unsigned int)atoi(buffer);
    return SUCCESS;
}