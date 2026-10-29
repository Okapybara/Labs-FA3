#include "functions4.h"

int main(){
    int n = 0;
    int capacity = 0;
    char *str = NULL;
    int str_len = 0;
    File *files = NULL;

    // чтение n и capacity
    while (1) {
        if (read_string(&str, &str_len) != SUCCESS) return 1;
        if (str_len == 0) {
            free(str);
            continue;
        }

        char *p = str;
        while (*p == ' ' || *p == '\t') p++;

        char *n_start = p;
        while (*p != ' ' && *p != '\t' && *p != '\0') p++;
        char saved1 = *p; 
        *p = '\0';
        
        if (read_int(n_start, &n) != SUCCESS || n <= 0 || n > MAX_FILES) {
            printf("Количество файлов должно быть целым положительным числом меньше %d. Введите строку заново (два числа - количество файлов и вместимость накопителя).\n", MAX_FILES);
            *p = saved1; 
            free(str);
            continue;
        }
        
        *p = saved1;
        while (*p == ' ' || *p == '\t') p++;
        char *cap_start = p;
        
        if (read_int(cap_start, &capacity) != SUCCESS || capacity < 0 || capacity > MAX_CAPACITY) {
            printf("Вместимость накопителя должна быть целым неотрицательным числом меньше %d. Введите строку заново (два числа - количество файлов и вместимость накопителя).\n", MAX_CAPACITY);
            free(str);
            continue;
        }

        free(str);
        break;
    }


    files = (File *)malloc(n * sizeof(File));
    if (files == NULL) {
        print_error(ERR_MEMORY);
        return 1;
    }


    // чтение файлов
    for (int i = 0; i < n; ) {
        if (read_string(&str, &str_len) != SUCCESS) {
            for (int j = 0; j < i; j++) free(files[j].name);
            free(files);
            return 1;
        }

        if (str_len == 0) {
            free(str);
            continue;
        }

        char *p = str;
        while (*p == ' ' || *p == '\t') p++;

        // читаем имя файла
        char *name_start = p;
        while (*p != ' ' && *p != '\t' && *p != '\0') p++;
        int name_len = p - name_start;

        if (name_len == 0) {
            printf("Имя файла не может быть пустым.\n");
            free(str);
            continue;
        }    

        files[i].name = (char *)malloc(name_len + 1);
        if (files[i].name == NULL) {
            print_error(ERR_MEMORY);
            free(str);
            return 1;
        }

        strncpy(files[i].name, name_start, name_len);
        files[i].name[name_len] = '\0';

        if (!is_name_unique(files[i].name, files, i)) {
            printf("Файл с именем '%s' уже существует.\n", files[i].name);
            free(files[i].name);
            free(str);
            continue;
        }


        // читаем размер файла - целое число
        while (*p == ' ' || *p == '\t') p++;
        char *size_start = p;
        while (*p != ' ' && *p != '\t' && *p != '\0') p++;
        
        char saved2 = *p;
        *p = '\0';
        
        int size = 0;
        if (read_int(size_start, &size) != SUCCESS || size <= 0) {
            printf("Размер файла должен быть целым положительным числом. Введите строку заново (файл, размер и важность).\n");
            free(files[i].name);
            *p = saved2; 
            free(str);
            continue;
        }
        files[i].size = size;

        // читаем capacity
        *p = saved2;
        while (*p == ' ' || *p == '\t') p++;
        char *imp_start = p;
        
        int importance = 0;
        if (read_int(imp_start, &importance) != SUCCESS || importance < 0) {
            printf("Важность должна быть целым неотрицательным числом. Введите строку заново (файл, размер и важность).\n");
            free(files[i].name);
            free(str);
            continue;
        }
        files[i].importance = importance;

        // переход к чтению след файла
        i++; 
        free(str);
    }

    Result res;
    status res_status = algorithm(files, n, capacity, &res);

    if (res_status != SUCCESS) {
        print_error(res_status);
        for (int i = 0; i < n; i++) free(files[i].name);
        free(files);
        return 1;
    }

    printf("\n");
    printf("TOTAL_SIZE %d\n", res.total_size);
    printf("TOTAL_IMPORTANCE %d\n", res.total_importance);
    printf("FILES %d\n", res.count);
    for (int i = 0; i < res.count; i++) printf("%s\n", res.selected_files[i]);

    for (int i = 0; i < res.count; i++) free(res.selected_files[i]);
    free(res.selected_files);

    for (int i = 0; i < n; i++) free(files[i].name);
    free(files);

    return 0;
}
