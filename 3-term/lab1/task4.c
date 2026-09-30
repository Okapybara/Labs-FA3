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


// новый путь
int output_path(const char *input_file, char **output_file){
    if (input_file == NULL || output_file == NULL) return INVALID;

    if (strchr(input_file, '\\') != NULL) return INVALID; // первое вхождение

    const char *last_slash = strrchr(input_file, '/'); // возвращает адрес последнего

    size_t dir_len = 0;
    const char *filename = input_file;

    if (last_slash != NULL){
        dir_len = (size_t)(last_slash - input_file + 1); // адрес слэша - адрес начала массива входного файла = кол-во символов со слэшем
        filename = last_slash + 1; // указатель на начало имени файла и вправо
    }

    if (strlen(filename) == 0) return INVALID;


    size_t new_len = dir_len + 4 + strlen(filename) + 1;

    char *new_out = (char*) malloc(new_len);
    if (new_out == NULL) return INVALID; 

    if (dir_len > 0) memcpy(new_out, input_file, dir_len); // байты соединяет
    memcpy(new_out + dir_len, "out_", 4);
    strcpy(new_out + dir_len + 4, filename);

    *output_file = new_out;
    return SUCCESS;
}

// сравнение имен
int same_files(const char *input_file, const char *output_file ){
    if (input_file == NULL || output_file == NULL) return INVALID;

    if (strchr(input_file, '\\') != NULL || strchr(output_file, '\\') != NULL) return INVALID;

    const char *input_last_slash = strrchr(input_file, '/');
    const char *output_last_slash = strrchr(output_file, '/');

    const char *input_filename = input_file;
    const char *output_filename = output_file;

    if (input_last_slash != NULL) input_filename = input_last_slash + 1;
    if (output_last_slash != NULL) output_filename = output_last_slash + 1;

    if (strlen(input_filename) == 0 || strlen(output_filename) == 0) return INVALID;
    
    if (strcmp(input_filename, output_filename) == 0) return SAME;
    else return NOT_SAME;
}



int FlagD(FILE *fin, FILE *fout){
    if (fin == NULL || fout == NULL) return INVALID;

    int ch;
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return READ_ERROR;
        if (!isdigit(ch)){
            if (fputc(ch, fout) == EOF) return WRITE_ERROR;
        }
    }
    
    return SUCCESS;
} 


int FlagI(FILE *fin, FILE *fout){
    if (fin == NULL || fout == NULL) return INVALID;

    int ch;
    int count = 0;
    int last_ch = 0;
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return READ_ERROR;
        last_ch = ch;
        

        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) count++;    
        if (ch == '\n'){
            if (fprintf(fout, "%d\n", count) < 0) return WRITE_ERROR;
            count = 0;
        }
    }

    if (last_ch != '\n'){
        if (fprintf(fout, "%d\n", count) < 0) return WRITE_ERROR;
    } 

    return SUCCESS;
} 


int FlagS(FILE *fin, FILE *fout){
    if (fin == NULL || fout == NULL) return INVALID;

    int ch;
    int count = 0;
    int last_ch = 0;
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return READ_ERROR;
        last_ch = ch;

        if ((ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z') && !isdigit(ch) &&
            ch != ' '&& ch != '\n' && ch != '\r') count++;    
        if (ch == '\n'){
            if (fprintf(fout, "%d\n", count) < 0) return WRITE_ERROR;
            count = 0;
        }
    }

    if (last_ch != '\n'){
        if (fprintf(fout, "%d\n", count) < 0) return WRITE_ERROR;
    } 

    return SUCCESS;
} 


int FlagA(FILE *fin, FILE *fout){ // доделать
    if (fin == NULL || fout == NULL) return INVALID;

    int ch;
    char hex[] = "0123456789ABCDEF";
    while ((ch = fgetc(fin)) != EOF){
        if (ferror(fin)) return READ_ERROR;

        if (!isdigit(ch)){
            int high = ch/16;
            int low = ch%16;
            if (fputc(hex[high], fout) == EOF) return WRITE_ERROR;
            if (fputc(hex[low], fout) == EOF) return WRITE_ERROR;
        }
        else{
            if (fputc(ch, fout) == EOF) return WRITE_ERROR;
        }
    }    

    return SUCCESS;
} 



int main(int argc, char *argv[]){
    if (argc < 3 || argc > 4){
        printf("Некорректный ввод. Напишите %s <flag> <input_file> [output_file].\n", argv[0]);
        return 1;
    }

    char *flag = argv[1];

    if (flag[0] != '-' && flag[0] != '/'){
        printf("Некорректный ввод флага.");
        return 1;
    }

    char *input_file = argv[2];
    char *out_file = NULL;
    char flag_letter;
    int has_n = 0;

    if (strlen(flag) == 2){
        flag_letter = flag[1];
        if (argc != 3) {
            printf("Некорректный ввод флага. Для флага без 'n' требуется 3 аргумента.\n");
            return 1;
        }
    }

    else if (strlen(flag) == 3 && flag[1] == 'n'){
        flag_letter = flag[2];
        has_n = 1;
        if (argc != 4) {
            printf("Некорректный ввод флага. Для флага с 'n' требуется 4 аргумента.\n");
            return 1;
        }
    }

    else {
        printf("Некорректный ввод флага.\n");
        return 1;
    }


    if (flag_letter != 'd' && flag_letter != 'i' && flag_letter != 's' && flag_letter != 'a') {
        printf("Неизвестный флаг.\n");
        return 1;
    }


    if (has_n)
        out_file = argv[3];
    else{
        if (output_path(input_file, &out_file) != SUCCESS){
            printf("Некорректный путь к входному файлу.\n");
            return 1;
        }
    }

    if (same_files(input_file, out_file) == SAME) {
        printf("Входной и выходной файлы совпадают.\n");
        if (!has_n) free(out_file);
        return 1;
    }


    FILE *fin = fopen(input_file, "r");
    if (fin == NULL) {
        printf("Не удалось открыть входной файл '%s'.\n", input_file);
        if (!has_n) free(out_file);
        return 1;
    }

    FILE *fout = fopen(out_file, "w");
    if (fout == NULL) {
        printf("Не удалось открыть выходной файл '%s'.\n", out_file);
        fclose(fin);
        if (!has_n) free(out_file);
        return 1;
    }

    int status = SUCCESS;
    switch (flag_letter){
    case 'd':
        status = FlagD(fin, fout);
        break;

    case 'i':
        status = FlagI(fin, fout);
        break;
    
    case 's':
        status = FlagS(fin, fout);
        break;
    
    case 'a':
        status = FlagA(fin, fout);
        break;

    default:
        printf("Неизвестный флаг.\n");
        status = INVALID;
        break;
    }

    if (status != SUCCESS){
        printf("Ошибка. Файл не обработан.\n");
        fclose(fin);
        fclose(fout);
        if (!has_n) free(out_file);
        return 1;
    }
    
    fclose(fin);
    fclose(fout);
    if (!has_n) free(out_file);
    return 0;
}
