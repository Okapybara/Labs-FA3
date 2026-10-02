#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define SUCCESS 0
#define READ_ERROR 1
#define WRITE_ERROR 2
#define INVALID 3

#define SAME 1
#define NOT_SAME 0

#define MAX_WORD_LEN 4000

// сравнение имен
int same_files(const char *file1, const char *file2 ){
    if (file1 == NULL || file2 == NULL) return INVALID;

    if (strchr(file1, '\\') != NULL || strchr(file2, '\\') != NULL) return INVALID;

    const char *file1_last_slash = strrchr(file1, '/');
    const char *file2_last_slash = strrchr(file2, '/');

    const char *file1_filename = file1;
    const char *file2_filename = file2;

    if (file1_last_slash != NULL) file1_filename = file1_last_slash + 1;
    if (file2_last_slash != NULL) file2_filename = file2_last_slash + 1;

    if (strlen(file1_filename) == 0 || strlen(file2_filename) == 0) return INVALID;
    
    if (strcmp(file1_filename, file2_filename) == 0) return SAME;
    else return NOT_SAME;
}


// вытаскивает и собирает лексему
int read_word(FILE *fin, char *str_res){
    if (fin == NULL || str_res == NULL) return READ_ERROR;

    int ch;
    int pos = 0;
    int too_long = 0;

    int first_ch = EOF;
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return READ_ERROR;
        if (ch != ' ' && ch != '\n' && ch != '\t'){
            first_ch = ch;
            break;
        }
    }
    if (first_ch == EOF) return EOF;

    str_res[pos] = (char)first_ch;
    pos++;

    while ((ch = fgetc(fin)) != EOF && (ch != ' ' && ch != '\n' && ch != '\t')){
        if (ferror(fin)) return READ_ERROR;
        if (pos < MAX_WORD_LEN-1){
            str_res[pos] = (unsigned char)ch;
            pos++;
        }
        else too_long = 1;
    }
    str_res[pos] = '\0';

    if (too_long) return INVALID;
    return SUCCESS;   
}


// перевод в другую сс
int to_other_base(const char *str, char *str_res, int str_res_max_len, int base){
    if (str == NULL || str_res_max_len <= 0 || str_res == NULL) return READ_ERROR;

    int pos = 0;
    int too_long = 0;
    for (int i = 0; str[i]!='\0'; i++){
        int val = (unsigned char)str[i];

        char temp[10];
        int temp_pos = 0;

        while (val > 0) {
                temp[temp_pos] = '0' + (val % base);
                temp_pos++;
                val /= base;
        }

        for (int j = temp_pos-1; j >= 0; j--){
            if (pos < str_res_max_len-1){
                str_res[pos] = temp[j];
                pos++;
            }
            else too_long = 1;
        }
    }
    str_res[pos] = '\0';
    if (too_long) return INVALID;
    return SUCCESS;   
}


int to_lower(char *str) {
    if (str == NULL) return INVALID;
    for (int i = 0; str[i] != '\0'; i++){
        str[i] = (char)tolower((unsigned char)str[i]);
    }
    return SUCCESS;
}


int FlagR(FILE *fin1, FILE *fin2, FILE *fout){
    if (fin1 == NULL || fin2 == NULL || fout == NULL) return INVALID;

    char word1 [MAX_WORD_LEN];
    char word2 [MAX_WORD_LEN];

    int first = 1;
    int end1 = 0;
    int end2 = 0;

    while (end1 == 0 || end2 == 0){
        if (end1 == 0){
            int status1 = read_word(fin1, word1);
            if (status1 == EOF) end1 = 1;
            else if (status1 != SUCCESS) return status1;
            else{
                if (!first) fprintf(fout, " ");
                fprintf(fout, "%s", word1);
                first = 0;
            }
        }

        if (end2 == 0){
            int status2 = read_word(fin2, word2);
            if (status2 == EOF) end2 = 1;
            else if (status2 != SUCCESS) return status2;
            else{
                if (!first) fprintf(fout, " "); // на случай если 1 файл пустой
                fprintf(fout, "%s", word2);
                first = 0;
            }
        }
    }

    return SUCCESS;
}


int FlagA(FILE *fin1, FILE *fout){
    if (fin1 == NULL || fout == NULL) return INVALID;

    char word[MAX_WORD_LEN];
    char new_word[MAX_WORD_LEN * 4];

    int count = 0;
    int first = 1;

    while (1){
        int status = read_word(fin1, word);
        if (status == EOF) break;
        if (status != SUCCESS) return status;

        count++;

        if (count % 10 == 0){
            to_lower(word);
            to_other_base(word, new_word, sizeof(new_word), 4);
        }

        else if(count % 5 == 0 && count % 10 != 0)
            to_other_base(word, new_word, sizeof(new_word), 8);

        else if (count % 2 == 0 && count % 10 != 0)
            to_lower(word);
        
        if (!first) fprintf(fout, " ");
        if (count % 10 == 0 || count % 5 == 0) fprintf(fout, "%s", new_word);
        else fprintf(fout, "%s", word);

        first = 0;
    }

    return SUCCESS;
}


int main(int argc, char *argv[]){
    if (argc < 4 || argc > 5){
        printf("Некорректный ввод. Напишите %s <flag> <file1> <file2> [file3].\n", argv[0]);
        return 1;
    }

    char *flag = argv[1];

    if (strlen(flag) < 2 || (flag[0] != '-' && flag[0] != '/')){
        printf("Некорректный ввод флага.\n");
        return 1;
    }

    char flag_letter = flag[1];
    if (flag_letter != 'r' && flag_letter != 'a') {
        printf("Неизвестный флаг.\n");
        return 1;
    }

    if (flag_letter == 'r' && argc != 5){
        printf("Некорректное количество параметров для флага 'r'.\n");
        return 1;
    }

    if (flag_letter == 'a' && argc != 4){
        printf("Некорректное количество параметров для флага 'a'.\n");
        return 1;
    }

    char *file1 = argv[2];
    char *file2 = (flag_letter == 'r') ? argv[3] : NULL;
    char *out_file = (flag_letter == 'r') ? argv[4] : argv[3];
    if (flag_letter == 'r'){
        if (same_files(file1, out_file) == SAME || same_files(file2, out_file) == SAME){
            printf("Файлы совпадают.\n");
            return 1;
        }
    }
    else{
        if (same_files(file1, out_file) == SAME){
            printf("Файлы совпадают.\n");
            return 1;
        }
    }


    FILE *fin1 = fopen(file1, "r");
    if (fin1 == NULL) {
        printf("Не удалось открыть входной файл '%s'.\n", file1);
        return 1;
    }

    FILE *fin2 = NULL;
    if (flag_letter == 'r'){
        fin2 = fopen(file2, "r");
        if (fin2 == NULL) {
            printf("Не удалось открыть входной файл '%s'.\n", file2);
            fclose(fin1);
            return 1;
        }
    }


    FILE *fout = fopen(out_file, "w");
    if (fout == NULL) {
        printf("Не удалось открыть выходной файл '%s'.\n", out_file);
        fclose(fin1);
        if (fin2 != NULL) fclose(fin2);
        return 1;
    }

    int status = SUCCESS;
    switch (flag_letter){
    case 'r':
        status = FlagR(fin1, fin2, fout);
        break;

    case 'a':
        status = FlagA(fin1, fout);
        break;

    default:
        printf("Неизвестный флаг.\n");
        status = INVALID;
        break;
    }

    if (status != SUCCESS){
        printf("Ошибка. Файл не обработан.\n");
        fclose(fin1);
        if (fin2 != NULL) fclose(fin2);
        fclose(fout);
        return 1;
    }
    
    fclose(fin1);
    if (fin2 != NULL) fclose(fin2);
    fclose(fout);
    return 0;
}
