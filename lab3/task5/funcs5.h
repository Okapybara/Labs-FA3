#include "input5.h"

// задание
// сравнение среднего балла
int compare_doubles(double a, double b, double epsilon) {
    if (a < b - epsilon) return -1;
    if (a > b + epsilon) return 1;
    return 0;
}

// средний балл для 1 студента
double get_student_average(const Student *student) {
    if (student == NULL || student->marks == NULL) return 0.0;
    
    double sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += student->marks[i];
    }
    return sum / 5.0;
}

// общий средний балл
double get_overall_average(const Student *students, int count) {
    if (students == NULL || count <= 0) return 0.0;
    
    double total_sum = 0;
    for (int i = 0; i < count; i++) {
        total_sum += get_student_average(&students[i]);
    }
    return total_sum / count;
}


// компаратор по ID
int compare_by_id(const void *a, const void *b) {
    const Student *sa = (const Student *)a;
    const Student *sb = (const Student *)b;
    
    if (sa->id < sb->id) return -1;
    if (sa->id > sb->id) return 1;
    return 0;
}

// компаратор по фамилии
int compare_by_surname(const void *a, const void *b) {
    const Student *sa = (const Student *)a;
    const Student *sb = (const Student *)b;
    
    return strcmp(sa->surname, sb->surname);
}

// компаратор по имени
int compare_by_name(const void *a, const void *b) {
    const Student *sa = (const Student *)a;
    const Student *sb = (const Student *)b;
    
    return strcmp(sa->name, sb->name);
}

// компаратор по группе
int compare_by_group(const void *a, const void *b) {
    const Student *sa = (const Student *)a;
    const Student *sb = (const Student *)b;
    
    return strcmp(sa->group, sb->group);
}

// сортировка массива студентов: 1 - по ID, 2 - по фамилии, 3 - по имени, 4 - по группе
status sort_students(Student *students, int count, int sort_type) {
    if (students == NULL || count <= 0) return ERR_NULL;
    if (sort_type < 1 || sort_type > 4) return ERR_INVALID;
    
    int (*comparator)(const void *, const void *) = NULL;
    
    switch (sort_type) {
        case 1: comparator = compare_by_id; break;
        case 2: comparator = compare_by_surname; break;
        case 3: comparator = compare_by_name; break;
        case 4: comparator = compare_by_group; break;
    }

    qsort(students, count, sizeof(Student), comparator);
    
    return SUCCESS;
}

// вывод студентов - первые 10 записей
void print_students_preview(const Student *students, int count) {
    if (students == NULL || count <= 0) {
        printf("Список студентов пуст.\n");
        return;
    }
        
    printf("\n");
    printf("Первые <=10 из %d:\n", count);
    
    int limit = (count < 10) ? count : 10;
    
    for (int i = 0; i < limit; i++) {
        printf("%d) ID: %u; %s %s; группа: %s; оценки: %d %d %d %d %d\n", i+1, students[i].id, students[i].name, students[i].surname, students[i].group,
                                                                          students[i].marks[0], students[i].marks[1], students[i].marks[2], 
                                                                          students[i].marks[3], students[i].marks[4]);    
    }
    
    if (count > 10) {
        printf("... и еще %d студентов.\n", count - 10);
    }
}

status save_all_students_to_file(const char *filename, const Student *students, int count) {
    if (filename == NULL || students == NULL) return ERR_NULL;
    if (count <= 0) return ERR_INVALID;
    
    FILE *fout = fopen(filename, "w");
    if (fout == NULL) return ERR_READ;
    
    for (int i = 0; i < count; i++) {
        fprintf(fout, "%d) ID: %u; %s %s; группа: %s; оценки: %d %d %d %d %d\n", i+1, students[i].id, students[i].name, students[i].surname, students[i].group,
                                                                          students[i].marks[0], students[i].marks[1], students[i].marks[2], 
                                                                          students[i].marks[3], students[i].marks[4]);    
    }

    fclose(fout);
    return SUCCESS;
}


// копирование студента
status copy_student(const Student *source, Student *dest) {
    if (source == NULL || dest == NULL) return ERR_NULL;
    
    dest->id = 0;
    dest->name = NULL;
    dest->surname = NULL;
    dest->group = NULL;
    dest->marks = NULL;
    
    dest->id = source->id;
    
    dest->name = (char *)malloc((strlen(source->name) + 1) * sizeof(char));
    if (dest->name == NULL) return ERR_MEMORY;
    strcpy(dest->name, source->name);
    
    dest->surname = (char *)malloc((strlen(source->surname) + 1) * sizeof(char));
    if (dest->surname == NULL) {
        free_student(dest);
        return ERR_MEMORY;
    }
    strcpy(dest->surname, source->surname);
    
    dest->group = (char *)malloc((strlen(source->group) + 1) * sizeof(char));
    if (dest->group == NULL) {
        free_student(dest);
        return ERR_MEMORY;
    }
    strcpy(dest->group, source->group);
    
    dest->marks = (unsigned char *)malloc(5 * sizeof(unsigned char));
    if (dest->marks == NULL) {
        free_student(dest);
        return ERR_MEMORY;
    }
    
    for (int i = 0; i < 5; i++) {
        dest->marks[i] = source->marks[i];
    }
    
    return SUCCESS;
}


// поиск по ID
status find_by_id(const Student *students, int count, unsigned int id, Student **out_found, int *out_found_count) {
    if (students == NULL || out_found == NULL || out_found_count == NULL) return ERR_NULL;
    
    *out_found = NULL;
    *out_found_count = 0;
    
    int found_count = 0;
    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            found_count++;
        }
    }
    
    if (found_count == 0) return ERR_NOT_FOUND;
    
    Student *found = (Student *)malloc(found_count * sizeof(Student));
    if (found == NULL) return ERR_MEMORY;
    
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            status st = copy_student(&students[i], &found[idx]);
            if (st != SUCCESS) {
                for (int j = 0; j < idx; j++) free_student(&found[j]);
                free(found);
                return st;
            }
            idx++;
        }
    }
    
    *out_found = found;
    *out_found_count = found_count;
    return SUCCESS;
}

// поиск по фамилии
status find_by_surname(const Student *students, int count, const char *surname, Student **out_found, int *out_found_count) {
    if (students == NULL || surname == NULL || out_found == NULL || out_found_count == NULL) 
        return ERR_NULL;
    
    *out_found = NULL;
    *out_found_count = 0;
    
    int found_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].surname, surname) == 0) {
            found_count++;
        }
    }
    
    if (found_count == 0) return ERR_NOT_FOUND;
    
    Student *found = (Student *)malloc(found_count * sizeof(Student));
    if (found == NULL) return ERR_MEMORY;
    
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].surname, surname) == 0) {
            status st = copy_student(&students[i], &found[idx]);
            if (st != SUCCESS) {
                for (int j = 0; j < idx; j++) free_student(&found[j]);
                free(found);
                return st;
            }
            idx++;
        }
    }
    
    *out_found = found;
    *out_found_count = found_count;
    return SUCCESS;
}

// поиск по имени
status find_by_name(const Student *students, int count, const char *name, Student **out_found, int *out_found_count) {
    if (students == NULL || name == NULL || out_found == NULL || out_found_count == NULL) 
        return ERR_NULL;
    
    *out_found = NULL;
    *out_found_count = 0;
    
    int found_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            found_count++;
        }
    }
    
    if (found_count == 0) return ERR_NOT_FOUND;
    
    Student *found = (Student *)malloc(found_count * sizeof(Student));
    if (found == NULL) return ERR_MEMORY;
    
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            status st = copy_student(&students[i], &found[idx]);
            if (st != SUCCESS) {
                for (int j = 0; j < idx; j++) free_student(&found[j]);
                free(found);
                return st;
            }
            idx++;
        }
    }
    
    *out_found = found;
    *out_found_count = found_count;
    return SUCCESS;
}

// поиск по группе
status find_by_group(const Student *students, int count, const char *group, Student **out_found, int *out_found_count) {
    if (students == NULL || group == NULL || out_found == NULL || out_found_count == NULL) 
        return ERR_NULL;
    
    *out_found = NULL;
    *out_found_count = 0;
    
    int found_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].group, group) == 0) {
            found_count++;
        }
    }
    
    if (found_count == 0) return ERR_NOT_FOUND;
    
    Student *found = (Student *)malloc(found_count * sizeof(Student));
    if (found == NULL) return ERR_MEMORY;
    
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].group, group) == 0) {
            status st = copy_student(&students[i], &found[idx]);
            if (st != SUCCESS) {
                for (int j = 0; j < idx; j++) free_student(&found[j]);
                free(found);
                return st;
            }
            idx++;
        }
    }
    
    *out_found = found;
    *out_found_count = found_count;
    return SUCCESS;
}


// запись студента со средним баллом по id в файл
status save_id_student_to_file(const char *filename, const Student *students, int count) {
    if (filename == NULL || students == NULL) return ERR_NULL;
    if (count <= 0) return ERR_INVALID;
    
    FILE *fout = fopen(filename, "w");
    if (fout == NULL) return ERR_READ;
    
    for (int i = 0; i < count; i++) {
        double avg = get_student_average(&students[i]);
        fprintf(fout, "%d) ID: %u; %s %s; группа: %s; средний балл: %.5f\n", i+1, students[i].id, students[i].name, 
                                                                                 students[i].surname, students[i].group, avg);    
    }

    fclose(fout);
    return SUCCESS;
}

// запись списка студентов со средним баллом выше общего среднего
status save_above_average_to_file(const char *filename, const Student *students, int count) {
    if (filename == NULL || students == NULL) return ERR_NULL;
    if (count <= 0) return ERR_INVALID;
    
    FILE *fout = fopen(filename, "w");
    if (fout == NULL) return ERR_READ;
    
    double overall_avg = get_overall_average(students, count);
    int written_count = 0;
    
    for (int i = 0; i < count; i++) {
        double student_avg = get_student_average(&students[i]);
        
        if (compare_doubles(student_avg, overall_avg, EPSILON) > 0) {
            fprintf(fout, "%s %s\n", students[i].surname, students[i].name);
            written_count++;
        }
    }
    
    fclose(fout);
    if (written_count == 0) return ERR_NOT_FOUND;
    return SUCCESS;
}
