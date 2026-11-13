#include "funcs5.h"

void preview() {
    printf("1. Найти студента по ID\n");
    printf("2. Найти студентов по фамилии\n");
    printf("3. Найти студентов по имени\n");
    printf("4. Найти студентов по группе\n");
    printf("5. Отсортировать весь список по ID\n");
    printf("6. Отсортировать весь список по фамилии\n");
    printf("7. Отсортировать весь список по имени\n");
    printf("8. Отсортировать весь список по группе\n");
    printf("9. Записать в трассировочный файл студентов со средним баллом выше общего\n");
    printf("0. Выход из программы\n");
    printf("Введите номер команды: ");
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Использование: %s <input.txt> <output.txt>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    if (text_extension(input_filename) != SUCCESS) {
        print_error(ERR_NOT_TEXT);
        return 1;
    }

    if (text_extension(output_filename) != SUCCESS) {
        print_error(ERR_NOT_TEXT);
        return 1;
    }

    if (same_files(input_filename, output_filename) == SAME_FILES) {
        print_error(SAME_FILES);
        return 1;
    }

    FILE *fin = fopen(input_filename, "r");
    if (fin == NULL) {
        print_error(ERR_READ);
        return 1;
    }

    Student *students = NULL;
    int count = 0;
    
    status st = read_students(fin, &students, &count);
    fclose(fin);
    if (st != SUCCESS) {
        print_error(st);
        return 1;
    }

    while (1) {
        preview();
        unsigned int choice;
        status st_in = read_num(&choice);
        if (st_in == ERR_EOF) {
            printf("Конец ввода.\n");
            break;
        }

        if (st_in != SUCCESS || choice > 9) {
            printf("Неверный ввод. Пожалуйста, введите число от 0 до 9.\n\n");
            continue;
        }

        if (choice == 0) {
            printf("Завершение работы программы.\n");
            break;
        }

        switch (choice) {
            case 1: {
                unsigned int id;
                printf("Введите ID студента: ");
                if (read_num(&id) != SUCCESS) {
                    printf("Неверный ввод ID. Введите корректное неотрицательное число.\n\n");
                    break;
                }

                Student *found = NULL;
                int found_count = 0;
                st = find_by_id(students, count, id, &found, &found_count);

                if (st == SUCCESS) {
                    printf("Найдено совпадений: %d\n", found_count);
                    print_students_preview(found, found_count);
                    
                    st = save_id_student_to_file(output_filename, found, found_count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                    printf("\n\n");
                    free_students(found, found_count);
                } 
                else print_error(st);
                break;
            }

            case 2: {
                char *target = NULL;
                printf("Введите фамилию: ");
                status st_in = read_line_dynamic(stdin, &target);
                
                if (st_in != SUCCESS) {
                    printf("\n");
                    printf("Ошибка ввода.\n\n");
                    break;
                }

                if (target[0] == '\0') {
                    printf("Ввод не может быть пустым.\n\n");
                    free(target);
                    break;
                }
                
                Student *found = NULL;
                int found_count = 0;
                st = find_by_surname(students, count, target, &found, &found_count);
                
                free(target);

                if (st == SUCCESS) {
                    printf("Найдено совпадений: %d\n", found_count);
                    print_students_preview(found, found_count);
                    
                    st = save_all_students_to_file(output_filename, found, found_count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                    printf("\n\n");
                    free_students(found, found_count);
                } 
                else print_error(st);
                break;
            }

            case 3: {
                char *target = NULL;
                printf("Введите имя: ");
                status st_in = read_line_dynamic(stdin, &target);
                
                if (st_in != SUCCESS) {
                    printf("\n");
                    printf("Ошибка ввода.\n\n");
                    break;
                }

                if (target[0] == '\0') {
                    printf("Ввод не может быть пустым.\n\n");
                    free(target);
                    break;
                }
                
                Student *found = NULL;
                int found_count = 0;
                st = find_by_name(students, count, target, &found, &found_count);
                
                free(target);

                if (st == SUCCESS) {
                    printf("Найдено совпадений: %d\n", found_count);
                    print_students_preview(found, found_count);
                    
                    st = save_all_students_to_file(output_filename, found, found_count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                    printf("\n\n");
                    free_students(found, found_count);
                } 
                else print_error(st);
                break;
            }

            case 4: {
                char *target = NULL;
                printf("Введите группу: ");
                status st_in = read_line_dynamic(stdin, &target);
                
                if (st_in != SUCCESS) {
                    printf("\n");
                    printf("Ошибка ввода.\n\n");
                    break;
                }

                if (target[0] == '\0') {
                    printf("Ввод не может быть пустым.\n\n");
                    free(target);
                    break;
                }
                
                Student *found = NULL;
                int found_count = 0;
                st = find_by_group(students, count, target, &found, &found_count);
                
                free(target);

                if (st == SUCCESS) {
                    printf("Найдено совпадений: %d\n", found_count);
                    print_students_preview(found, found_count);

                    st = save_all_students_to_file(output_filename, found, found_count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                    printf("\n\n");
                    free_students(found, found_count);
                } 
                else print_error(st);
                break;
            }

            case 5:
                st = sort_students(students, count, 1);
                if (st == SUCCESS) {
                    printf("Список отсортирован по ID.\n");
                    print_students_preview(students, count);
                    
                    st = save_all_students_to_file(output_filename, students, count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                }
                else print_error(st);
                break;

            case 6:
                st = sort_students(students, count, 2);
                if (st == SUCCESS) {
                    printf("Список отсортирован по фамилии.\n");
                    print_students_preview(students, count);

                    st = save_all_students_to_file(output_filename, students, count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                }
                else print_error(st);
                break;

            case 7:
                st = sort_students(students, count, 3);
                if (st == SUCCESS) {
                    printf("Список отсортирован по имени.\n");
                    print_students_preview(students, count);

                    st = save_all_students_to_file(output_filename, students, count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                }
                else print_error(st);
                break;

            case 8:
                st = sort_students(students, count, 4);
                if (st == SUCCESS) {
                    printf("Список отсортирован по группе.\n");
                    print_students_preview(students, count);

                    st = save_all_students_to_file(output_filename, students, count);
                    if (st == SUCCESS) printf("Данные успешно записаны в файл.\n");
                    else print_error(st);
                }
                else print_error(st);
                break;

            case 9: {
                st = save_above_average_to_file(output_filename, students, count);
                if (st == SUCCESS) printf("Данные успешно записаны в файл.\n\n");
                else if (st == ERR_NOT_FOUND) printf("Нет студентов со средним баллом выше общего среднего.\n\n");
                else print_error(st);
                break;
            }

            default: printf("Неверный номер команды. Попробуйте снова.\n\n"); break;
        }
        printf("\n");
    }

    free_students(students, count);
    return 0;
}
