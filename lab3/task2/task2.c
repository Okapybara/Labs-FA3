#include "functions2.h"

int main(){
    int n = 0;
    int k = 0;
    char line[MAX_LINE_SIZE];
    Report *reports = NULL;

    if (read_string(line, MAX_LINE_SIZE) != SUCCESS) {
        print_error(ERR_INVALID);
        return 1;
    }

    const char *p = line;
    
    // чтение n
    while (*p == ' ' || *p == '\t') p++;
    const char *n_start = p;
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    int n_len = p - n_start;
    
    if (n_len == 0 || n_len > 15) {
        print_error(ERR_INVALID);
        return 1;
    }

    char n_str[20];
    strncpy(n_str, n_start, n_len);
    n_str[n_len] = '\0';
    
    if (read_int(n_str, &n) != SUCCESS) {
        print_error(ERR_INVALID);
        return 1;
    }

    // чтение k
    while (*p == ' ' || *p == '\t') p++;
    const char *k_start = p;
    while (*p != ' ' && *p != '\t' && *p != '\0') p++;
    int k_len = p - k_start;
    
    if (k_len == 0 || k_len > 15) {
        print_error(ERR_INVALID);
        return 1;
    }
    char k_str[20];
    strncpy(k_str, k_start, k_len);
    k_str[k_len] = '\0';
    
    if (read_int(k_str, &k) != SUCCESS) {

        print_error(ERR_INVALID);
        return 1;
    }

    while (*p == ' ' || *p == '\t') p++;
    if (*p != '\0') {
        print_error(ERR_INVALID);
        return 1;
    }

    if (n <= 0 || n > MAX_N) {
        printf("Число заявок должно быть от 1 до %d.\n", MAX_N);
        print_error(ERR_OVERFLOW);
        return 1;
    }

    if (k <= 0 || k > MAX_K) {
        printf("Число доступных залов должно быть от 1 до %d.\n", MAX_K);
        print_error(ERR_OVERFLOW);
        return 1;
    }


    // заявки
    reports = (Report *)malloc(n * sizeof(Report));
    if (reports == NULL) {
        print_error(ERR_MEMORY);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        if (read_string(line, MAX_LINE_SIZE) != SUCCESS) {
            print_error(ERR_INVALID);
            free(reports);
            return 1;
        }

        status parse_status = report_line(line, &reports[i], reports, i);
        if (parse_status != SUCCESS) {
            print_error(parse_status);
            free(reports);
            return 1;
        }
    }

    int max_reports = 0;
    status res_reports = schedule_reports(reports, n, k, &max_reports);
    if (res_reports != SUCCESS) {
        print_error(res_reports);
        free(reports);
        return 1;
    }

    printf("%d\n", max_reports);

    free(reports);
    return 0;
}
