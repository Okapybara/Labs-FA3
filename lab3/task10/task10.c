#include "functions10.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_error(ERR_INVALID_ARGC);
        printf("Ввод %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    if (text_extension(argv[1]) != SUCCESS || text_extension(argv[2]) != SUCCESS) {
        print_error(ERR_NOT_TEXT);
        return 1;
    }

    if (same_files(argv[1], argv[2]) == SAME_FILES) {
        print_error(SAME_FILES);
        return 1;
    }

    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        print_error(ERR_READ);
        return 1;
    }

    FILE *out_file = fopen(argv[2], "w");
    if (out_file == NULL) {
        fclose(in_file);
        print_error(ERR_READ);
        return 1;
    }

    char *line = NULL;
    int line_len = 0;
    int tree_counter = 1;
    status read_status;

    while ((read_status = read_file(in_file, &line, &line_len)) == SUCCESS) {
        if (validate_line(line) != SUCCESS) {
            printf("Ошибка валидации выражения в дереве номер %d.\n", tree_counter);
            free(line);
            fclose(in_file);
            fclose(out_file);
            return 1;
        }

        Node *root = NULL;
        int parse_index = 0;
        status parse_status = parse_node(line, &parse_index, &root);

        if (parse_status != SUCCESS) {
            printf("Ошибка парсинга в строке %d.\n", tree_counter);
            free(line);
            fclose(in_file);
            fclose(out_file);
            return 1;
        }

        fprintf(out_file, "Дерево %d:\n", tree_counter);
        print_tree(out_file, root, 0);
        fprintf(out_file, "\n");

        tree_counter++;

        free_tree(root);
        free(line);
        line = NULL;
        line_len = 0;
    }

    if (read_status != ERR_READ) {
        print_error(read_status);
        fclose(in_file);
        fclose(out_file);
        return 1;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
