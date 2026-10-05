#include "functions8.h"

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Некорректный ввод. Напишите %s <in_file> <out_file>.\n", argv[0]);
        return 1;
    }

    char *in_file = argv[1];
    char *out_file = argv[2];
    
    if (same_files(in_file, out_file) == SAME_FILES){
        printf("Файлы совпадают.\n");
        return 1;
    }

    if (text_extension(in_file) == NOT_TEXT || text_extension(out_file) == NOT_TEXT){
        printf("Файл не является текстовым.\n");
        return 1;
    }

    FILE *fin = fopen(in_file, "r");
    if (fin == NULL) {
        printf("Не удалось открыть входной файл '%s'.\n", in_file);
        return 1;
    }

    if (extra_symbols_file(fin) != SUCCESS){
        printf("Содержание файла некорректно.\n");
        fclose(fin);
        return 1;
    }

    rewind(fin); // перевод курсора в начало

    FILE *fout = fopen(out_file, "w");
    if (fout == NULL) {
        printf("Не удалось открыть выходной файл '%s'.\n", out_file);
        fclose(fin);
        return 1;
    }

    char number[MAX_WORD_LEN]; // сюда число
    int count = 0; // считает количество считанных слов
    
    // запись в выходной файл
    while (1) {
        status s = read_number(fin, number);
                
        if (s == EMPTY_FILE) break;

        if (s != SUCCESS) {
            printf("Ошибка при чтении числа.\n");
            fclose(fin);
            fclose(fout);
            return 1;
        }

        delete_leading_zeros(number);
        to_lower(number);

        int base = 0;
        if (min_base(number, &base) != SUCCESS) {
            printf("Ошибка определения минимального основания.\n");
            fclose(fin);
            fclose(fout);
            return 1;
        }

        unsigned long long value_ten = 0;
        if (to_base_ten(number, base, &value_ten) != SUCCESS) {
            printf("Ошибка перевода числа '%s' в десятичную систему счисления.\n", number);
            fclose(fin);
            fclose(fout);
            return 1;
        }

        count++;

        if (fprintf(fout, "%d) %s %d %llu\n", count, number, base, value_ten) < 0) {
            printf("Не удалось записать данные в выходной файл.\n");
            fclose(fin);
            fclose(fout);
            return 1;
        }
    }

    if (count == 0) printf("Файл не содержит чисел для обработки.\n");

    fclose(fin);
    fclose(fout);
    return 0;
}
