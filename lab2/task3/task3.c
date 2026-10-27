#include "functions3.h"

int main(int argc, char *argv[]){
    if (argc < 3){
        print_error(ERR_INVALID_ARGC);
        printf("Использование: %s <кол-во файлов от 1 до 5> <фaйл1.txt> [фaйл2.txt] ...\n", argv[0]);
        return 1;
    }

    int file_count = 0;
    status res = read_int(argv[1], &file_count);
    if (res != SUCCESS) {
        print_error(res);
        return 1;
    }

    if (file_count < 1 || file_count > MAX_FILES) {
        print_error(ERR_FILE_COUNT);
        return 1;
    }

    if (argc != 2 + file_count) {
        printf("Неверно передано количество файлов. Заявлено %d файлов, передано %d файлов.\n", file_count, argc - 2);
        print_error(ERR_INVALID_ARGC);
        return 1;
    }

    char filenames[MAX_FILES][MAX_PATH_LEN];
    for (int i = 0; i < file_count; ++i) {
        char *arg_file = argv[2 + i];
        
        if (strlen(arg_file) >= MAX_PATH_LEN) {
            printf("Ошибка в аргументе %d: путь слишком длинный.\n", i + 2);
            print_error(ERR_INVALID_NAME);
            return 1;
        }

        strcpy(filenames[i], arg_file);

        // Проверка расширения
        if (text_extension(filenames[i]) != SUCCESS) {
            printf("Ошибка в имени файла %d ('%s'): ", i + 2, filenames[i]);
            print_error(ERR_NOT_TEXT);
            return 1;
        }
    }

    char *search_str = NULL;
    int search_len = 0;
    
    status sub_res = read_substring(&search_str, &search_len);
    if (sub_res != SUCCESS) {
        print_error(sub_res);
        return 1;
    }

    status search_res = search_in_files(search_str, search_len, file_count, filenames[0],
                                           filenames[1], filenames[2], filenames[3], filenames[4]);

    if (search_res != SUCCESS) {
        print_error(search_res);
        free(search_str);
        return 1;
    }

    free(search_str);
    return 0;
}
