#include <stdio.h>
#include <stdlib.h>
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
    SAME_FILES, // одинаковый входной и выходной файл
} status;

#define INITIAL_BUF_SIZE 64

// узел дерева
typedef struct Node {
    char *name;
    struct Node *first_child; // первый ребенок
    struct Node *next_sibling; // брат - след ребенок того же родителя
} Node;


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

// чтение строки файла
status read_file(FILE *file, char **buf, int *len) {
    if (file == NULL || buf == NULL || len == NULL) return ERR_NULL;

    int capacity = INITIAL_BUF_SIZE;
    int length = 0;
    int chars_read = 0;

    char *buffer = (char *)malloc(capacity * sizeof(char));
    if (buffer == NULL) return ERR_MEMORY;

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\r') continue;
        if (ch == '\n') break;

        if (length >= capacity - 1) {
            if (capacity > INT_MAX / 2) {
                free(buffer);
                return ERR_MEMORY;
            }
            capacity *= 2;
            char *new_buf = (char *)realloc(buffer, capacity * sizeof(char));
            if (new_buf == NULL) {
                free(buffer);
                return ERR_MEMORY;
            }
            buffer = new_buf;
        }
        buffer[length++] = (char)ch;
        chars_read++;
    }

    if (chars_read == 0 && ch == EOF) {
        free(buffer);
        return ERR_READ;
    }
    
    buffer[length] = '\0';

    *buf = buffer;
    *len = length;
    return SUCCESS;
}

int is_valid_name(unsigned char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    if (c >= '0' && c <= '9') return 1;
    return 0;
}

int find_prev_significant(const char *line, int i) {
    while (i > 0) {
        i--;
        if (line[i] != ' ') return i;
    }
    return -1;
}

int find_next_significant(const char *line, int i, int len) {
    for (int j = i + 1; j < len; j++) {
        if (line[j] != ' ') return j;
    }
    return -1;
}

// проверка на корректность строки
status validate_line(const char *line) {
    if (line == NULL) return ERR_NULL;
    
    size_t len = strlen(line);
    if (len == 0) return ERR_INVALID;

    int balance = 0;

    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)line[i];
        if (c == ' ') continue;

        if (!is_valid_name(c) && c != '(' && c != ')' && c != ',') return ERR_INVALID;

        if (c == '(') balance++;
        else if (c == ')') {
            balance--;
            if (balance < 0) return ERR_INVALID;
        } 
        else if (c == ',') {
            if (i == 0 || i == len - 1) return ERR_INVALID;
            int prev_idx = find_prev_significant(line, i);
            int next_idx = find_next_significant(line, i, len);

            if (prev_idx == -1) return ERR_INVALID;
            if (next_idx == -1) return ERR_INVALID;
            if (line[prev_idx] == '(') return ERR_INVALID;
            if (line[next_idx] == ')') return ERR_INVALID;
            if (line[next_idx] == ',') return ERR_INVALID;
        }
    }

    if (balance != 0) return ERR_INVALID;

    return SUCCESS;
}


// функции для работы с деревом

Node* create_node(const char *name, size_t len) {
    if (name == NULL) return NULL;

    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->name = (char *)malloc((len + 1) * sizeof(char));
    if (node->name == NULL) {
        free(node);
        return NULL;
    }

    memcpy(node->name, name, len);
    node->name[len] = '\0';
    node->first_child = NULL;
    node->next_sibling = NULL;

    return node;
}

void free_tree(Node *root) {
    if (root == NULL) return;
    free_tree(root->first_child);
    free_tree(root->next_sibling);
    free(root->name);
    free(root);
}

void print_tree(FILE *out, const Node *root, int depth) {
    if (out == NULL || root == NULL) return;

    for (int i = 0; i < depth; i++) {
        fprintf(out, "  ");
    }
    fprintf(out, "%s\n", root->name);

    print_tree(out, root->first_child, depth + 1);
    print_tree(out, root->next_sibling, depth);
}

// функция
status parse_node(const char *str, int *index, Node **out_node) {
    if (str == NULL || index == NULL || out_node == NULL) return ERR_NULL;

    while (str[*index] == ' ') (*index)++;

    int start = *index;

    while (str[*index] != '\0' && is_valid_name((unsigned char)str[*index])) {
        (*index)++;
    }

    size_t len = (size_t)(*index - start);
    if (len == 0) return ERR_INVALID;

    Node *node = create_node(str + start, len);
    if (node == NULL) return ERR_MEMORY;

    while (str[*index] == ' ') (*index)++;

    if (str[*index] == '(') {
        (*index)++;

        Node *first_child = NULL;
        Node *last_child = NULL;

        while (str[*index] != ')' && str[*index] != '\0') {
            Node *child = NULL;
            status res = parse_node(str, index, &child);
            if (res != SUCCESS) {
                free_tree(node);
                return res;
            }

            if (first_child == NULL) {
                first_child = child;
                last_child = child;
            } 
            else {
                last_child->next_sibling = child;
                last_child = child;
            }

            while (str[*index] == ' ') (*index)++;

            if (str[*index] == ',') (*index)++;
            else if (str[*index] != ')') {
                free_tree(node);
                return ERR_INVALID;
            }
        }

        if (str[*index] != ')') {
            free_tree(node);
            return ERR_INVALID;
        }

        node->first_child = first_child;
        (*index)++;
    }

    *out_node = node;
    return SUCCESS;
}
