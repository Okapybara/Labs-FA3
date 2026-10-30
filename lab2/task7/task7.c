#include "functions7.h"

int main() {
    int n = 0, count = 0;
    char *str = NULL;
    int str_len = 0;

    // размерность
    while (1) {
        printf("Введите размерность пространства n (n > 0): ");
        status res = read_string(&str, &str_len);
        if (res != SUCCESS) {
            print_error(res);
            return 1;
        }

        if (str_len > 0 && read_int(str, &n) == SUCCESS && n > 0 && n <= MAX_N) {
            free(str);
            break;
        }
        
        printf("Неверно введена размерность. Введите целое положительное число (от 1 до %d).\n", MAX_N);
        free(str);
    }

    // количество векторов
    while (1) {
        printf("Введите количество векторов (от 1 до %d): ", MAX_VECTORS);
        status res = read_string(&str, &str_len);
        if (res != SUCCESS) {
            print_error(res);
            return 1;
        }

        if (str_len > 0 && read_int(str, &count) == SUCCESS && count > 0 && count <= MAX_VECTORS) {
            free(str);
            break;
        }
        
        printf("Неверно введено количество векторов. Введите число от 1 до %d.\n", MAX_VECTORS);
        free(str);
    }

    // ввод координат векторов
    printf("\n");
    Vector *vectors = (Vector *)calloc(MAX_VECTORS, sizeof(Vector));
    if (vectors == NULL) {
        print_error(ERR_MEMORY);
        return 1;
    }

    for (int i = 0; i < count;) {
        printf("Введите %d координат для вектора %d (через пробел): ", n, i + 1);
        if (read_string(&str, &str_len) != SUCCESS) {
            for (int j = 0; j < i; j++) free(vectors[j].coords);
            free(vectors);
            return 1;
        }

        if (str_len == 0) {
            free(str);
            continue;
        }

        vectors[i].n = n;
        vectors[i].coords = (double *)malloc(n * sizeof(double));
        if (vectors[i].coords == NULL) {
            print_error(ERR_MEMORY);
            for (int j = 0; j < i; j++) free(vectors[j].coords);
            free(vectors);
            free(str);
            return 1;
        }

        char *p = str;
        int coord = 0;

        while (coord < n) {
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '\0') break;

            char *start = p;
            while (*p != ' ' && *p != '\t' && *p != '\0') p++;
            
            char saved = *p;
            *p = '\0';

            if (read_double(start, &vectors[i].coords[coord]) != SUCCESS) {
                printf("Координата должна быть числом. Введите строку заново.\n");
                *p = saved; 
                coord = -1;
                break;
            }
            
            *p = saved;
            coord++;
        }

        if (coord != n) {
            if (coord != -1) {
                printf("Необходимо ввести ровно %d координат. Введите строку заново.\n", n);
            }
            free(vectors[i].coords); 
            free(str);
            continue;
        }

        while (*p == ' ' || *p == '\t') p++;
        if (*p != '\0') {
            printf("Введено слишком много координат. Введите ровно %d чисел.\n", n);
            free(vectors[i].coords);
            free(str);
            continue;
        }

        i++; 
        free(str);
    }

    // генерация диагональной матрицы для 3 нормы - положительно определенная
    double **A = (double **)malloc(n * sizeof(double *));
    if (A == NULL) {
        print_error(ERR_MEMORY);
        for (int j = 0; j < count; j++) free(vectors[j].coords);
        free(vectors);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        A[i] = (double *)calloc(n, sizeof(double));
        if (A[i] == NULL) {
            print_error(ERR_MEMORY);
            for (int j = 0; j < i; j++) free(A[j]);
            free(A);
            for (int j = 0; j < count; j++) free(vectors[j].coords);
            free(vectors);
            return 1;
        }
        A[i][i] = (double)(i + 1);
    }


    // количество норм    
    int norm_count = 0;
    while (1) {
        printf("\n");
        printf("Сколько норм хотите проверить (от 1 до %d)? ", MAX_NORMS);
        status res_norm = read_string(&str, &str_len);
        if (res_norm != SUCCESS) {
            print_error(res_norm);
            for (int i = 0; i < count; i++) free(vectors[i].coords);
            free(vectors);
            for (int i = 0; i < n; i++) free(A[i]);
            free(A);
            return 1;
        }

        if (str_len > 0 && read_int(str, &norm_count) == SUCCESS && norm_count >= 1 && norm_count <= MAX_NORMS) {
            free(str);
            break;
        }
        printf("Неверно введено количество. Введите число от 1 до %d.\n", MAX_NORMS);
        free(str);
    }


    // чтение норм
    char norm_names[MAX_NORMS][64]; 
    int p_values[MAX_NORMS];
    int p_count = 0;

    Norm selected_norms[MAX_NORMS];
    int has_inf = 0;
    int has_A = 0;

    printf("\n");
    for (int i = 0; i < norm_count; ) {
        int choice = 0;
        printf("Выберите тип нормы для проверки %d:\n", i + 1);
        printf("1) ||x||_inf (максимальная координата)\n");
        printf("2) ||x||_p (степенная норма)\n");
        printf("3) ||x||_A (норма с матрицей)\n");
        printf("Ваш выбор (1, 2 или 3): ");
        
        status res_choice = read_string(&str, &str_len);
        if (res_choice != SUCCESS) { 
            print_error(res_choice);
            for (int j = 0; j < count; j++) free(vectors[j].coords);
            free(vectors);
            for (int j = 0; j < n; j++) free(A[j]);
            free(A);
            return 1; 
        }

        if (str_len > 0 && read_int(str, &choice) == SUCCESS && (choice == 1 || choice == 2 || choice == 3)) {
            free(str);

            if (choice == 1) {
                if (has_inf) {
                    printf("Норма ||x||_inf уже выбрана. Введите другую.\n\n");
                    continue;
                }
                selected_norms[i].func = norm_inf;
                selected_norms[i].params = NULL;
                selected_norms[i].name = "||x||_inf";
                has_inf = 1;
                i++;
            }

            else if (choice == 3) {
                if (has_A) {
                    printf("Норма ||x||_A уже выбрана. Введите другую.\n\n");
                    continue;
                }
                selected_norms[i].func = norm_A;
                selected_norms[i].params = A;
                selected_norms[i].name = "||x||_A";
                has_A = 1;
                i++;
            }

            else if (choice == 2) {
                int current_p = 0;
                while (1) {
                    printf("Введите параметр p для нормы ||x||_p (p >= 1, например 2): ");
                    status res = read_string(&str, &str_len);
                    if (res != SUCCESS) {
                        print_error(res);
                        for (int j = 0; j < count; j++) free(vectors[j].coords);
                        free(vectors);
                        for (int j = 0; j < n; j++) free(A[j]);
                        free(A);
                        return 1; 
                    }

                    if (str_len > 0 && read_int(str, &current_p) == SUCCESS && current_p >= 1 && current_p <= MAX_P) {
                        free(str);
                        break;
                    }
                    
                    printf("Неверно введен параметр p. Параметр p должен быть числом >= 1 до %d. Введите заново.\n", MAX_P);
                    free(str);
                }

                int p_exists = 0;
                for (int k = 0; k < i; k++) {
                    if (selected_norms[k].func == norm_p && *(int *)selected_norms[k].params == current_p) {
                        p_exists = 1;
                        break;
                    }
                }

                if (p_exists) {
                    printf("Норма с p = %d уже добавлена. Введите другое значение p.\n\n", current_p);
                    continue;
                }

                p_values[p_count] = current_p;
                selected_norms[i].func = norm_p;
                selected_norms[i].params = &p_values[p_count]; 
            
                // для каждой p-нормы свое имя
                selected_norms[i].name = norm_names[i]; 
                snprintf(selected_norms[i].name, 64, "||x||_p (p = %d)", current_p);
                
                p_count++;
                i++;
            }
        } 
        else {
            printf("Неверный выбор. Введите 1, 2 или 3.\n\n");
            free(str);
        }
    }

    printf("\n");
    
    status res_status = find_longest_vectors(n, count, norm_count, &vectors[0], &vectors[1], 
                                             &vectors[2], &vectors[3], &vectors[4], &vectors[5], 
                                             &vectors[6], &vectors[7], &vectors[8], &vectors[9], 
                                             &selected_norms[0], &selected_norms[1], &selected_norms[2], 
                                             &selected_norms[3], &selected_norms[4]);

    if (res_status != SUCCESS) {
        print_error(res_status);

        for (int i = 0; i < count; i++) free(vectors[i].coords);
        free(vectors);

        for (int i = 0; i < n; i++) free(A[i]);
        free(A);

        return 1;
    }


    for (int i = 0; i < count; i++) free(vectors[i].coords);
    free(vectors);

    for (int i = 0; i < n; i++) free(A[i]);
    free(A);

    return 0;
}
