#include "functions2.h"

int main(int argc, char *argv[]){
    if (argc != 4){
        print_error(ERR_INVALID_ARGC);
        return 1;
    }

    char *in_file = argv[1];
    char* flag = argv[2];
    char *out_file = argv[3];

    if ((flag[0] != '-' && flag[0] != '/') || flag[1] == '\0' || flag[2] != '\0') {
        print_error(ERR_INVALID_FLAG);
        return 1;
    }

    char flag_letter = flag[1];
    if (flag_letter != 'a' && flag_letter != 'd') {
        print_error(ERR_INVALID_FLAG);
        return 1;
    }

    if (same_files(in_file, out_file) == SAME_FILES) {
        print_error(SAME_FILES);
        return 1;
    }

    if (text_extension(in_file) == NOT_TEXT || text_extension(out_file) == NOT_TEXT) {
        print_error(NOT_TEXT);
        return 1;
    }

    FILE *fin = fopen(in_file, "r");
    if (fin == NULL) {
        printf("Не удалось открыть входной файл '%s'.\n", in_file);
        return 1;
    }

    Employee *array = NULL;
    int count = 0;
    status res = read_employees(fin, &array, &count);
    fclose(fin);

    if (res != SUCCESS) {
        print_error(res);
        return 1;
    }

    res = sort(array, count, flag_letter);
    if (res != SUCCESS) {
        print_error(res);
        free(array);
        return 1;
    }


    FILE *fout = fopen(out_file, "w");
    if (fout == NULL) {
        printf("Не удалось открыть выходной файл '%s'.\n", out_file);
        free(array);
        return 1;
    }

    res = write_employees(fout, array, count);
    fclose(fout);

    if (res != SUCCESS){
        print_error(res);
        free(array);
        return 1;
    }

    free(array);
    return 0;
}
