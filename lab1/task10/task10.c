#include "functions10.h"


int main(int argc, char *argv[]){
    if (argc != 1){
        printf("Некорректный ввод. Напишите %s.\n", argv[0]);
        return 1;
    }

    char base_str[5];
    printf("Введите основание системы счисления (от 2 до 36): ");
    
    if (scanf("%4s", base_str) != 1) {
        printf("Ошибка чтения ввода.\n");
        return 1;
    } 
    
    int next = getchar();
    if (next != ' ' && next != '\n' && next != '\t' && next != '\r' && next != EOF) {
        printf("Ошибка чтения ввода. Переполнение.\n");
        return 1;
    }

    // проверка правильного ввода основания системы счисления
    if (base_str[0] == '\0'){
        printf("Неправильно введено число.\n");
        return 1;
    }
    
    for (int i = 0; base_str[i] != '\0'; i++){
        if (!isdigit((unsigned char)base_str[i])){
            printf("Неправильно введено число.\n");
            return 1;
        }
    }

    if (Overflow(base_str)){
        printf("Переполнение.\n");
        return 1;
    }

    
    int base = atoi(base_str); // основание сс
    if (base < 2 || base > 36) {
        printf("Основание системы счисления должно быть от 2 до 36.\n");
        return 1;
    }

    char number[20]; // читает число
    int isstop = 0;
    
    unsigned long long sum = 0;
    unsigned long long max_val = 0;
    int count = 0; // прочитанные

    printf("Введите числа (для завершения введите Stop):\n");

    while (1) {
        status s = read_number(number, &isstop);
        if (s == ERR_OVERFLOW) {
            printf("Введено слишком длинное число.\n");
            return 1;
        }
        if (s != SUCCESS) {
            printf("Ошибка чтения ввода.\n");
            return 1;
        }
        if (isstop) break;

        if (correct_number(number, base) != SUCCESS) {
            printf("Число '%s' содержит недопустимые символы для системы счисления с основанием %d.\n", number, base);
            return 1;
        }

        delete_leading_zeros(number);

        unsigned long long value = 0;
        if (to_base_ten(number, base, &value) != SUCCESS) {
            printf("Переполнение при переводе числа '%s'.\n", number);
            return 1;
        }

        // максимальное и сумма
        if (count == 0 || value > max_val) max_val = value;
        if (sum > ULLONG_MAX - value) {
                printf("Переполнение суммы.\n");
                return 1;
        }

        sum += value;
        count++; // количество прочитанных
    }

    if (count == 0) {
        printf("Числа не были введены или прочитаны.\n");
        return 0;
    }

    // перевод в 9, 18, 27, 36
    char str_sum[65];
    char str_max[65];
    int bases[] = {9, 18, 27, 36};
    putchar('\n');
    printf("Результаты:\n");

    for (int i = 0; i < 4; i++) {
        int curr_base = bases[i];
        
        status s_sum = to_other_base(sum, str_sum, curr_base, 65);
        if (s_sum != SUCCESS) {
            printf("Ошибка при переводе суммы в систему счисления с основанием %d.\n", curr_base);
            return 1;
        }
        
        status s_max = to_other_base(max_val, str_max, curr_base, 65);
        if (s_max != SUCCESS) {
            printf("Ошибка при переводе максимума в систему счисления с основанием %d.\n", curr_base);
            return 1;
        }
        
        printf("Основание %d: Сумма = %s, максимум = %s\n", curr_base, str_sum, str_max);
    }


    return 0;
}
