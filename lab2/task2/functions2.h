#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef enum status {
    SUCCESS = 0, // успех
    ERR_INVALID_ARGC, // количество аргументов
    ERR_INVALID_FLAG, // проверка флага
    ERR_INVALID, // наличие неверных значений
    ERR_OVERFLOW, // переполнение
    ERR_NULL, // указатель нулевой
    ERR_MEMORY, // ошибка выделения памяти
    ERR_READ, // ошибка чтения
    ERR_WRITE, // ошибка записи
    ERR_INVALID_NAME, // неверный ввод имени файла
    EMPTY_FILE, // пустой файл (только из пробелов и тд)
    SAME_FILES, // одинаковые имена файлов
    NOT_TEXT // не текстовый файл
} status;

#define MAX_NAME_LEN 256
#define EPSILON 1e-9 // для вещественных чисел

typedef struct Employee {
    unsigned int id;
    char name[MAX_NAME_LEN];
    char surname[MAX_NAME_LEN];
    double salary;
} Employee;

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID_ARGC:
            printf("Неверное количество аргументов. Напишите ./a.out <in_file> <flag> <out_file>.\n");
            break;
        case ERR_INVALID_FLAG:
            printf("Неверный флаг. Можно только: -a, -d, /a, /d.\n");
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
        case ERR_READ:
            printf("Ошибка чтения файла.\n");
            break;
        case ERR_WRITE:
            printf("Ошибка записи в файл.\n");
            break;
        case ERR_INVALID_NAME:
            printf("Неверный ввод имени файла.\n");
            break;
        case EMPTY_FILE:
            printf("Файл не содержит данных.\n");
            break; 
        case SAME_FILES:
            printf("Имена входного и выходного файлов совпадают.\n");
            break; 
        case NOT_TEXT:
            printf("Файл не является текстовым.\n");
            break;
        default:
            printf("Неизвестная ошибка.\n");
            break;
    }
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


// проверяет расширение файлов
status text_extension(const char *file) {
    if (file == NULL) return ERR_NULL;

    const char *dot = strrchr(file, '.'); // последнее вхождение точки
    
    if (dot == NULL) return NOT_TEXT; // остаток строки от dot включительно
    if (strcmp(dot, ".txt") != 0 && strcmp(dot, ".TXT") != 0) return NOT_TEXT;
    
    return SUCCESS;
}

// переводит в unsigned int
status unsigned_int(const char *str, const char **endptr, unsigned int *id) {
    if (str == NULL || endptr == NULL || id == NULL) return ERR_NULL;

    const char *p = str;
    while (isspace((unsigned char)*p)) p++;

    if (!isdigit((unsigned char)*p)) return ERR_INVALID;

    unsigned long long val = 0;
    while (isdigit((unsigned char)*p)) {
        int digit = *p - '0';

        if (val > (UINT_MAX - digit) / 10) return ERR_OVERFLOW;
        val = val*10 + digit;
        p++;
    }

    if (!isspace((unsigned char)*p)) return ERR_INVALID;
    
    *id = (unsigned int)val;
    *endptr = p;
    return SUCCESS;
}

// переводит в double
status validate_double(const char *str, const char **endptr, double *salary) {
    if (str == NULL || endptr == NULL || salary == NULL) return ERR_NULL;

    const char *p = str;

    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return ERR_INVALID;

    char *double_endptr;
    double val = strtod(p, &double_endptr);

    if (double_endptr == p) return ERR_INVALID;

    if (isinf(val) || isnan(val)) return ERR_INVALID;
    if (val < 0.0) return ERR_INVALID;

    p = double_endptr;
    
    while (isspace((unsigned char)*p)) p++;
    if (*p != '\0') return ERR_INVALID;

    *salary = val;
    *endptr = p;
    return SUCCESS;
}

// проверяет слово имя/фамилию
status validate_word(const char *str, const char **endptr, char *word, int max_len) {
    if (str == NULL || endptr == NULL || word == NULL) return ERR_NULL;
    if (max_len == 0) return ERR_INVALID;

    const char *p = str;
    int len = 0;

    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return ERR_INVALID;

    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            if (len >= max_len - 1) return ERR_OVERFLOW;
            
            word[len] = (char)c;
            len++;
            p++;
        }
        else break;
    }

    if (len == 0) return ERR_INVALID;

    word[len] = '\0';
    *endptr = p;
    return SUCCESS;
}

// кладет в структуру employee строку
status validate_employee(const char *line, Employee *emp) {
    if (line == NULL || emp == NULL) return ERR_NULL;

    const char *p = line;
    const char *next_p = NULL;

    status res = unsigned_int(p, &next_p, &emp->id);
    if (res != SUCCESS) return res;
    p = next_p;

    res = validate_word(p, &next_p, emp->name, MAX_NAME_LEN);
    if (res != SUCCESS) return res;
    p = next_p;

    res = validate_word(p, &next_p, emp->surname, MAX_NAME_LEN);
    if (res != SUCCESS) return res;
    p = next_p;

    res = validate_double(p, &next_p, &emp->salary);
    if (res != SUCCESS) return res;
    p = next_p;

    if (*p != '\0') return ERR_INVALID; 
    return SUCCESS;
}

// кладет в динамический массив построчно
status read_employees(FILE *fin, Employee **out_array, int *out_count) {
    if (fin == NULL || out_array == NULL || out_count == NULL) return ERR_NULL;

    Employee *array = NULL;
    int capacity = 0; // размер массива
    int count = 0; // считаем прочитанные элементы
    char line[1024];

    while (fgets(line, sizeof(line), fin) != NULL) {
        const char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;

        if (count == capacity) {
            int new_capacity = (capacity == 0) ? 4 : capacity * 2;
            Employee *temp = realloc(array, (size_t)new_capacity*sizeof(Employee));

            if (temp == NULL) {
                free(array);
                return ERR_MEMORY;
            }
            
            array = temp;
            capacity = new_capacity;
        }

        status res = validate_employee(line, &array[count]);
        
        if (res != SUCCESS) {
            free(array);
            return res;
        }

        count++;
    }

    if (ferror(fin)) {
        free(array);
        return ERR_READ;
    }

    if (count == 0) return EMPTY_FILE;

    *out_array = array;
    *out_count = count;
    return SUCCESS;
}

// для флага a
int FlagA(const void *a, const void *b) {
    const Employee *e1 = (const Employee *)a;
    const Employee *e2 = (const Employee *)b;

    if (fabs(e1->salary - e2->salary) > EPSILON)
        return (e1->salary > e2->salary) ? 1 : -1;

    int res = strcmp(e1->surname, e2->surname);
    if (res != 0) return res; 

    res = strcmp(e1->name, e2->name);
    if (res != 0) return res;

    if (e1->id > e2->id) return 1;
    if (e1->id < e2->id) return -1;

    return 0;
}

// для флага d
int FlagD(const void *a, const void *b) {
    return -FlagA(a, b);
}

// сортировка
status sort(Employee *array, int count, char flag) {
    if (array == NULL) return ERR_NULL;
    if (count <= 0) return SUCCESS;

    if (flag == 'a') qsort(array, count, sizeof(Employee), FlagA);
    else if (flag == 'd') qsort(array, count, sizeof(Employee), FlagD);

    else return ERR_INVALID_FLAG;

    return SUCCESS;
}

// запись в выходной файл
status write_employees(FILE *fout, const Employee *array, int count) {
    if (fout == NULL || array == NULL || count <= 0) return ERR_NULL;
    
    for (int i = 0; i < count; i++)
        fprintf(fout, "%u %s %s %.2f\n", array[i].id, array[i].name, array[i].surname, array[i].salary);

    if (ferror(fout)) return ERR_WRITE;

    return SUCCESS;
}
