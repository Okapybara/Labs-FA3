#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>


int prostoe(long n){
    if (n < 2) return 0;
    for (long i = 2; i <= n/i; i++){
        if (n%i==0) return 0;
    }

    return 1;
}


double e_lim(const double eps){
    double current = 0.0; 
    double prev = 0.0;
    long n = 1;

    do{
        if (n == LONG_MAX) break;
        prev = current;
        current = pow((1.0 + 1.0/(double)n), (double)n);
        n++;
    } while (fabs(current-prev) >= eps);

    return current;
}

double e_ser(const double eps){
    double sum = 1.0; // типо 1/0! уже прибавлен
    double chislo = 1.0; // типо 1/0! уже посчитан
    long n = 1;

    while(chislo >= eps){
        if (n == LONG_MAX) break;
        chislo /= (double)n; // вместо факториала
        sum += chislo;
        n++;
    }

    return sum;
} 

double e_equat(const double eps){
    double a = 2.0;
    double b = 3.0;
    double center = 0.0;

    while ((b-a) >= eps){
        center = (a+b)/2.0;

        if ((log(center)-1.0) <= 0.0) a = center;
        else b = center;
    }

    return (a+b)/2.0;
}



double pi_lim(const double eps){
    double current = 4.0; 
    double prev = 0.0;
    long n = 2;

    do{
        if (n == LONG_MAX) break;
        prev = current;
        double multipl = (4.0*(double)n*((double)n - 1.0))/((2.0*(double)n - 1.0)*(2.0*(double)n - 1.0));
        current *= multipl;
        n++;
    } while (fabs(current-prev) >= eps);

    return current;
}

double pi_ser(const double eps){
    double sum = 0.0;
    double chislo = 0.0;
    double sign = 1.0;
    long n = 1;

    do{
        if (n == LONG_MAX) break;

        chislo = 4.0/(2.0*(double)n - 1.0);

        sum += (sign*chislo);
        sign = -sign;

        n++;
    } while (chislo >= eps);
    
    return sum;
} 

double pi_equat(const double eps){
    double a = 3.0;
    double b = 4.0;
    double center = 0.0;

    while ((b-a) >= eps){
        center = (a+b)/2.0;

        double fa = cos(a) + 1.0;
        double fb = cos(b) + 1.0;

        if (fa < fb-eps) b = center; // двигаемся к нулю
        else a = center;
    }

    return (a+b)/2.0;
}



double ln_lim(const double eps){
    double current = 0.0;
    double prev = 0.0;
    long n = 1;

    do{
        if (n == LONG_MAX) break;
        prev = current;
        current = (double)n * (pow(2.0, (1.0/(double)n)) - 1.0);
        n++;
    } while (fabs(current-prev) >= eps);

    return current;
}

double ln_ser(const double eps){
    double sum = 0.0;
    double chislo = 0.0;
    double sign = 1.0;
    long n = 1;

    do{
        if (n == LONG_MAX) break;

        chislo = 1.0/((double)n);

        sum += (sign*chislo);
        sign = -sign;

        n++;
    } while (chislo >= eps);
    
    return sum;
} 

double ln_equat(const double eps){
    double a = 0.0;
    double b = 1.0;
    double center = 0.0;

    while ((b-a) >= eps){
        center = (a+b)/2.0;

        if ((exp(center)-2.0) <= 0.0) a = center;
        else b = center;
    }

    return (a+b)/2.0;
}


double sqrt_lim(const double eps){
    double current = -0.5; // дано в задании
    double prev = 0.0;
    long n = 1;

    do{
        if (n == LONG_MAX) break;
        prev = current;
        current = prev - (prev*prev)/2.0 + 1.0;
        n++;
    } while (fabs(current-prev) >= eps);

    return current;
}

double sqrt_ser(const double eps){
    double multipl = 1.0;
    double chislo = 0.0;
    double pow_two = 0.25; // тк k=2 на старте

    do{
        chislo = pow(2, pow_two);
        multipl *= chislo;
        pow_two /= 2.0;

    } while (chislo-1.0 >= eps); //-1 из за произведения
    
    return multipl;
} 

double sqrt_equat(const double eps){
    double a = 1.0;
    double b = 2.0;
    double center = 0.0;

    while ((b-a) >= eps){
        center = (a+b)/2.0;

        if (((center*center)-2.0) <= 0.0) a = center;
        else b = center;
    }

    return (a+b)/2.0;
}


double gamma_lim(const double eps){
    double current = 0.0; 
    double prev = 0.0;
    long m = 2; // так как ln(1) = 0 и при m=1 все будет равно 0

    do{
        if (m == 45) break;
        prev = current;
        current = 0.0; 

        double ln = 0.0; // ln(1) = 0
        double C_m_k = 1.0; // C_m_0 = 1

        for (int k = 1; k<=m; k++){ // разные суммы для каждого m
            C_m_k = C_m_k*(double)(m-k+1)/(double) k;

            ln += log((double)k); // log(ab) = log(a)+log(b)

            double sign = (k % 2 == 1) ? -1.0 : 1.0;

            current += C_m_k * (sign / (double) k) * ln;
        }
        if (isnan(current) || isinf(current)) return prev;
        m++;

    } while (fabs(current-prev) >= eps);

    return current;
}

double gamma_ser(const double eps){
    double sum = 0.0;
    double chislo = 0.0;
    double prev_sum = 0.0;
    long k = 2;
    double pi = acos(-1.0);
    long current_root = 1;

    do{
        if (k == LONG_MAX) break;
        long root = (long)floor(sqrt((double)k));
        
        chislo = 1.0/((double)root*root) - 1.0/(double)k;

        sum += chislo;


        // если след корень сделает так что число обнулится
        // проверка на то, что это целый корень типо 4, 9, 16 и тд
        // новые группы чисел больше почти ничего не добавляют к ответу, пора останавливаться
        // поэтому сравниваем суммы по блокам
        if (root > current_root) {
            if (fabs(sum - prev_sum) < eps) break;
            prev_sum = sum;
            current_root = root;
        }
        
        k++;
            
    } while (1);
    
    sum = -(pi*pi)/6 + sum;

    return sum;
} 

double gamma_equat(const double eps){
    double current = 0.0; 
    double prev = 0.0;
    long t = 2;
    double multipl = 1.0;

    do{
        if (t == LONG_MAX) break;
        prev = current;

        if (prostoe(t)) multipl *= (double)(t - 1) / (double)t;

        double value = log((double)t) * multipl;

        current = -log(value); // тк в уравнении e^(-x)=..., x=-ln(..)
        
        t++;
    } while (fabs(current-prev) >= eps);

    return current;
}



int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Некорректный ввод. Напишите %s <number>.\n", argv[0]);
        return 1;
    }

    if (*argv[1] == '\0'){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    char *endptr;
    double eps = strtod(argv[1], &endptr);

    if (*endptr != '\0'){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    if (isinf(eps) || isnan(eps)){
        printf("Некорректный ввод числа.\n");
        return 1;
    }

    if (eps <= 0.0 || eps > 1.0 ){
        printf("Некорректный ввод числа. Оно должно быть в диапазоне (0, 1].\n");
        return 1;
    }
    
    printf("-----------------------------------------------------------------\n");
    printf("          |    Предел    | Ряд / Произведение |    Уравнение     \n");
    printf("-----------------------------------------------------------------\n");
    printf("     %s    |  %.8f  |     %.8f     |    %.8f  \n", "e", e_lim(eps), e_ser(eps), e_equat(eps));
    printf("-----------------------------------------------------------------\n");
    printf("     %s   |  %.8f  |     %.8f     |    %.8f  \n", "pi", pi_lim(eps), pi_ser(eps), pi_equat(eps));
    printf("-----------------------------------------------------------------\n");
    printf("    %s  |  %.8f  |     %.8f     |    %.8f  \n", "ln 2", ln_lim(eps), ln_ser(eps), ln_equat(eps));
    printf("-----------------------------------------------------------------\n");
    printf("  %s |  %.8f  |     %.8f     |    %.8f  \n", "sqrt(2)", sqrt_lim(eps), sqrt_ser(eps), sqrt_equat(eps));
    printf("-----------------------------------------------------------------\n");
    printf("   %s  |  %.8f  |     %.8f     |    %.8f  \n", "gamma", gamma_lim(eps), gamma_ser(eps), gamma_equat(eps));
    printf("-----------------------------------------------------------------\n");
    

    return 0;
}
