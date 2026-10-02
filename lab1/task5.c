#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAX_ITERS_SUM 1000000
#define MAX_ITERS_INTEGRAL 20
#define ALMOST_ZERO 1e-12


// n = 0, inf; x^n / n!;
double summ_a(const double eps, const double x){
    double sum = 1.0; // первый сделан и прибавлен 
    double chislo = 1.0; // первый сделан x^0 = 1 / 0! = 1; 1/1=1
    int n = 1;

    while(fabs(chislo) >= eps && n < MAX_ITERS_SUM){
        chislo = chislo*x / (double)n;
        sum += chislo;
        n++;
    }

    if (n >= MAX_ITERS_SUM || isinf(sum) || isnan(sum)) return NAN; 

    return sum;
}

// n = 0, inf; ((-1)^n * x^(2n)) / (2n)!;
double summ_b(const double eps, const double x){
    double sum = 1.0; // первый сделан и прибавлен 
    double chislo = 1.0; // первый сделан x^(2*0) = 1 / (2*0)! = 1; 1*1/1 = 1
    int n = 1;

    while(fabs(chislo) >= eps && n < MAX_ITERS_SUM){
        chislo = (-chislo*x*x) / ((2.0*n)*(2.0*n-1.0));
        sum += chislo;
        n++;
    }

    if (n >= MAX_ITERS_SUM || isinf(sum) || isnan(sum)) return NAN; 

    return sum;
}


double summ_c(const double eps, const double x){
    double sum = 1.0; // первый сделан и прибавлен 
    double chislo = 1.0; // первый сделан = 1
    int n = 1;

    while(fabs(chislo) >= eps && n < MAX_ITERS_SUM){
        double nd = (double)n;
        chislo = (chislo*27.0*nd*nd*nd*x*x) / ((3.0*nd-2.0)*(3.0*nd-1.0)*3.0*nd);
        sum += chislo;
        n++;
    }

    if (n >= MAX_ITERS_SUM || isinf(sum) || isnan(sum)) return NAN; 

    return sum;
}


double summ_d(const double eps, const double x){
    double sum = (-x*x)/2.0; // первый сделан и прибавлен 
    double chislo = sum; // первый сделан
    int n = 2;

    while(fabs(chislo) >= eps && n < MAX_ITERS_SUM){
        chislo = (-chislo*(2.0*n-1)*x*x) / (2.0*n);
        sum += chislo;
        n++;
    }

    if (n >= MAX_ITERS_SUM || isinf(sum) || isnan(sum)) return NAN; 

    return sum;
}



double f_a(double x){
    if (fabs(x) < ALMOST_ZERO) return 1.0;
    return log(1+x)/x;
}

double f_b(double x){
    return exp(-(x*x)/2.0);
}

double f_c(double x){
    if (fabs(1.0 - x) < ALMOST_ZERO) return -log(ALMOST_ZERO);
    return -log(1-x);
}

double f_d(double x){
    if (fabs(x) < ALMOST_ZERO) return 1.0;
    return pow(x,x);
}


double integral_a(const double eps){
    double a = 0.0;
    double b = 1.0;
    long long n = 10; // разбиения

    // грубый счет первый
    double h = (b-a)/(double)n; // считаем ширину 1 кусочка по разбиению
    double I_old = 0.0;
    for (long long i = 0; i < n; i++)
        I_old += ((f_a(a + i * h)+f_a(a + (i+1) * h))*h)/2.0; // площади

    double I_new = 0.0;
    // считаем более точно сравнивая через эпсилон
    for (long long i = 0; i < MAX_ITERS_INTEGRAL; i++){
        n *= 2;
        h = (b-a)/(double)n;
        I_new = 0.0;

        for (long long j = 0; j < n; j++)
            I_new += ((f_a(a + j * h)+f_a(a + (j+1) * h))*h)/2.0;

        if (isnan(I_new) || isinf(I_new)) return NAN;

        if (fabs(I_new - I_old) < eps) return I_new;
        I_old = I_new;
    }

    return I_new;
}

double integral_b(const double eps){
    double a = 0.0;
    double b = 1.0;
    long long n = 10; // разбиения

    // грубый счет первый
    double h = (b-a)/(double)n; // считаем ширину 1 кусочка по разбиению
    double I_old = 0.0;
    for (long long i = 0; i < n; i++)
        I_old += ((f_b(a + i * h)+f_b(a + (i+1) * h))*h)/2.0; // площади

    double I_new = 0.0;
    // считаем более точно сравнивая через эпсилон
    for (long long i = 0; i < MAX_ITERS_INTEGRAL; i++){
        n *= 2;
        h = (b-a)/(double)n;
        I_new = 0.0;

        for (long long j = 0; j < n; j++)
            I_new += ((f_b(a + j * h)+f_b(a + (j+1) * h))*h)/2.0;

        if (isnan(I_new) || isinf(I_new)) return NAN;

        if (fabs(I_new - I_old) < eps) return I_new;
        I_old = I_new;
    }

    return I_new;
}

double integral_c(const double eps){
    double a = 0.0;
    double b = 1.0;
    long long n = 10; // разбиения

    // грубый счет первый
    double h = (b-a)/(double)n; // считаем ширину 1 кусочка по разбиению
    double I_old = 0.0;
    for (long long i = 0; i < n; i++)
        I_old += ((f_c(a + i * h)+f_c(a + (i+1) * h))*h)/2.0; // площади

    double I_new = 0.0;
    // считаем более точно сравнивая через эпсилон
    for (long long i = 0; i < MAX_ITERS_INTEGRAL; i++){
        n *= 2;
        h = (b-a)/(double)n;
        I_new = 0.0;

        for (long long j = 0; j < n; j++)
            I_new += ((f_c(a + j * h)+f_c(a + (j+1) * h))*h)/2.0;

        if (isnan(I_new) || isinf(I_new)) return NAN;

        if (fabs(I_new - I_old) < eps) return I_new;
        I_old = I_new;
    }

    return I_new;
}

double integral_d(const double eps){
    double a = 0.0;
    double b = 1.0;
    long long n = 10; // разбиения

    // грубый счет первый
    double h = (b-a)/(double)n; // считаем ширину 1 кусочка по разбиению
    double I_old = 0.0;
    for (long long i = 0; i < n; i++)
        I_old += ((f_d(a + i * h)+f_d(a + (i+1) * h))*h)/2.0; // площади

    double I_new = 0.0;
    // считаем более точно сравнивая через эпсилон
    for (long long i = 0; i < MAX_ITERS_INTEGRAL; i++){
        n *= 2;
        h = (b-a)/(double)n;
        I_new = 0.0;

        for (long long j = 0; j < n; j++)
            I_new += ((f_d(a + j * h)+f_d(a + (j+1) * h))*h)/2.0;

        if (isnan(I_new) || isinf(I_new)) return NAN;

        if (fabs(I_new - I_old) < eps) return I_new;
        I_old = I_new;
    }

    return I_new;
}



int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Некорректный ввод. Напишите %s <epsilon> <number>.\n", argv[0]);
        return 1;
    }

    if (*argv[1] == '\0' || *argv[2] == '\0'){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    char *endptr_eps;
    double eps = strtod(argv[1], &endptr_eps);

    char *endptr_x;
    double x = strtod(argv[2], &endptr_x);

    if (*endptr_eps != '\0' || *endptr_x != '\0'){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    if (isinf(eps) || isnan(eps) || isinf(x) || isnan(x)){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    if (eps <= 0.0 || eps > 1.0 ){
        printf("Некорректный ввод эпсилон. Оно должно быть в диапазоне (0, 1].\n");
        return 1;
    }
    
    double res_a = summ_a(eps, x);
    double res_b = summ_b(eps, x);
    double res_c = summ_c(eps, x);
    double res_d = summ_d(eps, x);

    double res_int_a = integral_a(eps);
    double res_int_b = integral_b(eps);
    double res_int_c = integral_c(eps);
    double res_int_d = integral_d(eps);

    printf("СУММЫ: \n");
    if (isnan(res_a)) 
        printf("a) Ряд не сошелся за %d итераций при данном x и эпсилон.\n", MAX_ITERS_SUM); 
    else printf("a) %.8f\n", res_a);

    if (isnan(res_b)) 
        printf("b) Ряд не сошелся за %d итераций при данном x и эпсилон.\n", MAX_ITERS_SUM); 
    else printf("b) %.8f\n", res_b);

    if (isnan(res_c)) 
        printf("c) Ряд не сошелся за %d итераций при данном x и эпсилон.\n", MAX_ITERS_SUM); 
    else printf("c) %.8f\n", res_c);

    if (isnan(res_d)) 
        printf("d) Ряд не сошелся за %d итераций при данном x и эпсилон.\n", MAX_ITERS_SUM); 
    else printf("d) %.8f\n", res_d);
    
    putchar('\n');
    printf("ИНТЕГРАЛЫ: \n");

    if (isnan(res_int_a)) 
        printf("a) Не достигнута заданная точность.\n");
    else printf("a) %.8f\n", res_int_a);
    
    if (isnan(res_int_b)) 
        printf("b) Не достигнута заданная точность.\n");
    else printf("b) %.8f\n", res_int_b);
    
    if (isnan(res_int_c)) 
        printf("c) Не достигнута заданная точность.\n");
    else printf("c) %.8f\n", res_int_c);
    
    if (isnan(res_int_d)) 
        printf("d) Не достигнута заданная точность.\n");
    else printf("d) %.8f\n", res_int_d);

    
    return 0;
}