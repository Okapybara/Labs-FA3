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
    ERR_DUPLICATE_ID // дубликат id
} status;

#define MAX_LINE_SIZE 256
#define MAX_N 10000
#define MAX_K 1000


typedef struct Report {
    int id;
    int start_time; // в минутах
    int finish_time;
} Report;

// вывод ошибок
void print_error(status err) {
    switch (err) {
        case ERR_INVALID: printf("Неверные данные.\n"); break;
        case ERR_NULL: printf("Нулевой указатель.\n"); break;
        case ERR_OVERFLOW: printf("Переполнение.\n"); break;
        case ERR_MEMORY: printf("Ошибка выделения памяти.\n"); break;
        case ERR_DUPLICATE_ID: printf("Повторяющийся идентификатор доклада.\n"); break; 
        default: printf("Неизвестная ошибка.\n"); break;
    }
}

// проверка уникальности id
status is_id_unique(const int new_id, const Report *reports, int count) {
    if (reports == NULL) return ERR_NULL;
    for (int i = 0; i < count; i++) {
        if (reports[i].id == new_id) {
            return ERR_DUPLICATE_ID;
        }
    }
    return SUCCESS;
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

    int val = atoi(str);

    *out_val = val;

    return SUCCESS;
}

// считывает данные
status read_string(char *buffer, int max_len) {
    if (buffer == NULL || max_len == 0) return ERR_NULL;
    
    if (fgets(buffer, max_len, stdin) == NULL) return ERR_INVALID;

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
        
        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
            len--;
        }
    }
    
    else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return ERR_OVERFLOW;
    }

    return SUCCESS;
}

// время в минуты
status read_time(const char *str, int *out_minutes) {
    if (str == NULL || out_minutes == NULL) return ERR_NULL;

    if (strlen(str) != 5) return ERR_INVALID;
    if (str[2] != ':') return ERR_INVALID;

    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit((unsigned char)str[i])) return ERR_INVALID;
    }

    int hours = (str[0] - '0') * 10 + (str[1] - '0');
    int minutes = (str[3] - '0') * 10 + (str[4] - '0');

    if (hours < 0 || hours > 23) return ERR_INVALID;
    if (minutes < 0 || minutes > 59) return ERR_INVALID;

    *out_minutes = hours * 60 + minutes;
    return SUCCESS;
}

// строки с докладами
status report_line(const char *line, Report *out_report, const Report *existing_reports, int count) {
    if (line == NULL || out_report == NULL) return ERR_NULL;

    // id доклада
    const char *p = line;

    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0') return ERR_INVALID;

    const char *id_start = p;
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    int id_len = p - id_start;
    
    if (id_len == 0 || id_len > 15) return ERR_INVALID;

    char id_str[20];
    strncpy(id_str, id_start, id_len);
    id_str[id_len] = '\0';

    int id;
    if (read_int(id_str, &id) != SUCCESS) return ERR_INVALID;
    if (id <= 0) return ERR_INVALID;
    if (is_id_unique(id, existing_reports, count) != SUCCESS) return ERR_DUPLICATE_ID;

    // время начала
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0') return ERR_INVALID;

    const char *start_start = p;
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    int start_len = p - start_start;

    if (start_len == 0 || start_len >= 20) return ERR_INVALID;
    char start_str[20];
    strncpy(start_str, start_start, start_len);
    start_str[start_len] = '\0';

    int start_min;
    if (read_time(start_str, &start_min) != SUCCESS) return ERR_INVALID;

    // время конца
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0') return ERR_INVALID;

    const char *finish_start = p;
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    int finish_len = p - finish_start;

    if (finish_len == 0 || finish_len >= 20) return ERR_INVALID;
    char finish_str[20];
    strncpy(finish_str, finish_start, finish_len);
    finish_str[finish_len] = '\0';

    int finish_min;
    if (read_time(finish_str, &finish_min) != SUCCESS) return ERR_INVALID;

    // проверка на концовку
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '\0') return ERR_INVALID;

    if (start_min >= finish_min) return ERR_INVALID;

    out_report->id = id;
    out_report->start_time = start_min;
    out_report->finish_time = finish_min;

    return SUCCESS;
}


int compare_reports(const void *a, const void *b) {
    const Report *ra = (const Report *)a;
    const Report *rb = (const Report *)b;

    if (ra->finish_time != rb->finish_time) {
        if (ra->finish_time < rb->finish_time) return -1;  // a раньше
        if (ra->finish_time > rb->finish_time) return  1;  // a позже
    }

    else if (ra->finish_time == rb->finish_time) {
        if (ra->start_time < rb->start_time) return -1;
        if (ra->start_time > rb->start_time) return  1;
    }

    return 0;
}

// основаная функция 
status schedule_reports(const Report *reports, int n, int k, int *out_max_reports) {
    if (out_max_reports == NULL) return ERR_NULL;
    if (reports == NULL) {
        *out_max_reports = 0;
        return ERR_NULL;
    }

    if (n <= 0 || k <= 0) {
        *out_max_reports = 0;
        return ERR_INVALID;
    }

    // копия массива
    Report *sorted_reports = (Report *)malloc(n * sizeof(Report));
    if (sorted_reports == NULL) {
        *out_max_reports = 0;
        return ERR_MEMORY;
    }

    for (int i = 0; i < n; i++) sorted_reports[i] = reports[i];

    // от самых ранних к самым поздним
    qsort(sorted_reports, n, sizeof(Report), compare_reports);

    if (k > n) k = n;
    int *halls = (int *)calloc(k, sizeof(int));
    if (halls == NULL) {
        free(sorted_reports);
        *out_max_reports = 0;
        return ERR_MEMORY;
    }

    int count = 0;
    for (int i = 0; i < n; i++) {
        int best_hall = 0;
        // ищем зал который освободится раньше всех
        for (int j = 1; j < k; j++) {
            if (halls[j] < halls[best_hall]) {
                best_hall = j;
            }
        }

        if (halls[best_hall] <= sorted_reports[i].start_time) {
            halls[best_hall] = sorted_reports[i].finish_time;
            count++;
        }
    }

    *out_max_reports = count;
    free(halls);
    free(sorted_reports);

    return SUCCESS;
}
