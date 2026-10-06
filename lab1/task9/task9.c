#include "functions9.h"


int main(int argc, char *argv[]){
    srand(time(NULL)); // для псевдослучайных чисел

    if (argc != 3) {
        printf("Некорректный ввод. Напишите %s <a> <b>.\n", argv[0]);
        return 1;
    }

    int a = 0;
    int b = 0;
    // проверка ввода
    for (int j = 1; j <= 2; j++) {
        int start = 0;
        if (argv[j][0] == '-' || argv[j][0] == '+'){
            start = 1;
        }

        if (argv[j][start] == '\0'){
            printf("Неправильно введено число.\n");
            return 1;
        }
        
        for (int i = start; argv[j][i] != '\0'; i++){
            if (!isdigit((unsigned char)argv[j][i])){
                printf("Неправильно введено число.\n");
                return 1; 
            }
        }

        if (Overflow(argv[j])){
            printf("Переполнение.\n");
            return 1;
        }

        if (j == 1) a = atoi(argv[j]);
        else b = atoi(argv[j]);
    }

    int flag_static = 0; // на ошибку статического массива
    int flag_dynamic = 0; // на ошибку динамического массива
    long long  range_ll = 0;

    printf("1) Статический массив:\n");
    if (b <= a){
        printf("Недопустимый массив. Второе число должно быть больше.\n");
        flag_static = 1;
    }
    // заранее проверяем на возможность генерации псевдослучайных чисел
    else {
        range_ll = (long long)b - (long long)a + 1; // чтобы не произошло переполнения
        if (range_ll > RAND_MAX) {
            printf("Диапазон [%d..%d] слишком большой для генератора случайных чисел.\n", a, b);
            flag_static = 1;
        }
    }

    
    // заполнение статического
    if (!flag_static){
        int static_arr[STATIC_SIZE];

        int range = (int)range_ll;
        for (int i = 0; i < STATIC_SIZE; i++) 
            static_arr[i] = a + (rand() % range); // rand генерирует от 0 до rand_max,
                                                  // у нас от a (a+) до b (%(b-a+1)) (range)
                                                  // соотв если диапазон будет больше rand_max, то будет ошибка

        // вывод для 1 номера
        printf("Исходный статический массив:\n");
        for (int i = 0; i < STATIC_SIZE; i++)
            printf("%d ", static_arr[i]);
        putchar('\n');

        int min_val = 0;
        int max_val = 0;

        if (static_array(static_arr, STATIC_SIZE, &min_val, &max_val) != SUCCESS) {
            printf("Ошибка при обработке статического массива.\n");
            return 1;
        }

        printf("Измененный статический массив (мин %d и макс %d поменяны местами):\n", min_val, max_val);
        for (int i = 0; i < STATIC_SIZE; i++)
            printf("%d ", static_arr[i]);
        putchar('\n');
    }

    printf("2) Динамические массивы:\n");





    if (flag_static == 1 && flag_dynamic == 1) {
        printf("\n Оба задания завершились с ошибками.\n");
        return 1;
    }

    return 0;
}
