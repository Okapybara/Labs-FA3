#include "functions6.h"


int main(){
    int choice;
    char buf_int[50];
    char buf_double[400];
    while (1) {
        print_menu();
        status st = read_int(buf_int, &choice);
        if (st != SUCCESS) {
            print_error(st);
            continue;
        }

        if (choice == 0) {
            printf("Выход из программы.\n");
            break;
        }

        switch (choice) {
            case 1:{
                putchar('\n');
                printf("1. Проверка многоугольника на выпуклость:\n");

                int n, result;
                // макс 5 вершин и 10 координат
                double coords[10] = {0.0}; 

                printf("Введите количество вершин (от 3 до 5): ");
                if (read_int(buf_int, &n) != SUCCESS || n < 3 || n > 5) {
                    print_error(ERR_INVALID);
                    break;
                }

                printf("Введите координаты вершин (x y):\n");
                int input_error = 0;
                for (int i = 0; i < n; i++) {
                    // для x
                    printf("Вершина %d (x): ", i+1);
                    if (read_double(buf_double, &coords[i*2]) != SUCCESS) {
                        print_error(ERR_INVALID);
                        input_error = 1; break;
                    }

                    // для y
                    printf("Вершина %d (y): ", i+1);
                    if (read_double(buf_double, &coords[i*2+1]) != SUCCESS) {
                        print_error(ERR_INVALID);
                        input_error = 1; break;
                    }
                }
                if (input_error) break;

                status st = func1(n, &result, coords[0], coords[1], coords[2], 
                                  coords[3], coords[4], coords[5], coords[6],
                                  coords[7], coords[8], coords[9]);
                
                if (st == SUCCESS) {
                    if (result == 1) printf("Многоугольник является выпуклым.\n");
                    else printf("Многоугольник является вогнутым.\n");
                } 
                else print_error(st);

                break;
            }

            case 2:{
                putchar('\n');
                printf("2. Значение многочлена в заданной точке:\n");

                double x, result;
                int n;

                printf("Введите точку x (вещественное число): ");
                if (read_double(buf_double, &x) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }

                printf("Введите степень многочлена (от 0 до 10): ");
                if (read_int(buf_int, &n) != SUCCESS || n < 0 || n > 10) {
                    print_error(ERR_INVALID);
                    break;
                }

                // для n нужно n+1 коэффициент. максимум 11 коэфф
                double coeffs[11] = {0.0}; 
                
                printf("Введите %d коэффициентов (от старшей степени x^%d до свободного члена x^0):\n", n + 1, n);
                int input_error = 0;
                for (int i = 0; i <= n; i++) {
                    printf("Коэффициент при x^%d: ",  n-i);
                    if (read_double(buf_double, &coeffs[i]) != SUCCESS) {
                        print_error(ERR_INVALID);
                        input_error = 1;
                        break;
                    }
                }
                if (input_error) break;

                status st = func2(x, n, &result, coeffs[0], coeffs[1], coeffs[2], coeffs[3], 
                                  coeffs[4], coeffs[5], coeffs[6], coeffs[7], coeffs[8], 
                                  coeffs[9], coeffs[10]);
                
                if (st == SUCCESS) printf("Значение многочлена P(%.5f) = %.5f\n", x, result);
                else print_error(st);
                
                break;
            }

            case 3:{
                putchar('\n');
                printf("3. Поиск чисел Капрекара:\n");

                int base, count;
                printf("Введите основание системы счисления (от 2 до 36): ");
                if (read_int(buf_int, &base) != SUCCESS || base < 2 || base > 36) {
                    print_error(ERR_INVALID);
                    break;
                }

                printf("Сколько чисел вы хотите проверить (от 1 до 5)? ");
                if (read_int(buf_int, &count) != SUCCESS || count < 1 || count > 5) {
                    print_error(ERR_INVALID);
                    break;
                }

                char numbers[5][50]; 

                printf("Введите %d чисел:\n", count);
                int input_error = 0;
                for (int i = 0; i < count; i++) {
                    printf("Число %d: ", i + 1);
                    if (scanf("%49s", numbers[i]) != 1) {
                        print_error(ERR_READ);
                        input_error = 1;
                        break;
                    }

                    int next_char = getchar();
                    if (next_char != '\n' && next_char != EOF) {
                        printf("Число слишком длинное (максимум 49 символов).\n");

                        int c;
                        while ((c = getchar()) != '\n' && c != EOF); // очищаем ввод
                        input_error = 1;
                        break;
                    }
                }
                if (input_error) break;

                status st = func3(base, count, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);
                
                if (st != SUCCESS) print_error(st);

                break;
            }

            case 4:{
                putchar('\n');
                printf("4. Среднее геометрическое:\n");
                int count;
                double arr[5] = {0.0};
                double result;

                printf("Сколько чисел вы хотите ввести (от 1 до 5)? ");
                if (read_int(buf_int, &count) != SUCCESS || count < 1 || count > 5) {
                    print_error(ERR_INVALID);
                    break;
                }

                printf("Введите %d вещественных чисел:\n", count);
                int input_error = 0;
                for (int i = 0; i < count; i++) {
                    printf("Число %d: ", i + 1);
                    if (read_double(buf_double, &arr[i]) != SUCCESS) {
                        print_error(ERR_INVALID);
                        input_error = 1;
                        break;
                    }
                }
                if (input_error) break;

                status st = func4(count, &result, arr[0], arr[1], arr[2], arr[3], arr[4]);
                
                if (st == SUCCESS) printf("Среднее геометрическое: %.5f\n", result);
                else print_error(st);
                
                break;
            }

            case 5:{
                putchar('\n');
                printf("5. Рекурсивное возведение в степень:\n");
                double x, result;
                int power;

                printf("Введите основание (вещественное число): ");
                if (read_double(buf_double, &x) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }

                printf("Введите степень (целое число): ");
                if (read_int(buf_int, &power) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }

                status st = func5(x, power, &result);
                if (st == SUCCESS) printf("Результат: %.5f ^ %d = %.5f\n", x, power, result);
                else print_error(st);

                break;
            }

            case 6:{
                putchar('\n');
                printf("6. Метод дихотомии:\n");
                printf("Доступные уравнения:\n");
                printf(" 1. x^2 - 4 = 0\n");
                printf(" 2. sin(x) = 0\n");
                printf(" 3. x^3 - x - 2 = 0\n");

                int eq_choice;
                printf("Выберите номер уравнения (1-3): ");
                if (read_int(buf_int, &eq_choice) != SUCCESS || eq_choice < 1 || eq_choice > 3) {
                    print_error(ERR_INVALID);
                    break;
                }

                double a, b, eps, result;
                printf("Введите левую границу a: ");
                if (read_double(buf_double, &a) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }
                
                printf("Введите правую границу b: ");
                if (read_double(buf_double, &b) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }
                
                printf("Введите точность eps: ");
                if (read_double(buf_double, &eps) != SUCCESS) {
                    print_error(ERR_INVALID);
                    break;
                }

                double (*chosen_func)(double) = NULL; // указатель на функцию
                if (eq_choice == 1) chosen_func = eq1;
                else if (eq_choice == 2) chosen_func = eq2;
                else if (eq_choice == 3) chosen_func = eq3;

                status st = func6(a, b, eps, chosen_func, &result);
                if (st == SUCCESS) printf("Найденный корень: x = %.5f\n", result);
                else print_error(st);

                break;
            }

            default: printf("Неверный выбор. Попробуйте снова.\n"); break;
        }
    }


    return 0;
}
