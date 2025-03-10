#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>



void FillInc(int A[], int n) {
    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }
}

void FillDec(int A[], int n) {
    for (int i = 0; i < n; i++) {
        A[i] = n - i;
    }
}

void FillRand(int A[], int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
}

int CheckCum(int A[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += A[i];
    }
    return sum;
}

int RunNumber(int A[], int n) {
    int runs = 1;
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) runs++;
    }
    return runs;
}

void PrintMas(int A[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%-3d ", A[i]);
    }
}

double Timer(void (*sortFunc)(int[], int, int*, int*), int A[], int n) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int comparisons, swaps;
    sortFunc(A, n, &comparisons, &swaps);
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

int KnuthStep(int n) {
    int h = 1;
    while ((h * 2) + 1 < n) {
        h = 2 * h + 1;
    }
    return h;
}

double expon=2.7182818284590452353602874713527;

int ehponenta(int n) {
    int h = 1;
    while ((h * expon) + 1 < n) {
        h = expon * h + 1;
    }
    return h;
}

void ShellSort(int arr[], int n, int *comparisons, int *swaps) {
    int h = KnuthStep(n);
    *comparisons = 0;
    *swaps = 0;
    while (h >= 1) {
        for (int i = h; i < n; i++) {
            (*swaps)++;
            int temp = arr[i];
            int j;
            for (j = i; j >= h && arr[j - h] > temp; j -= h) {
                (*swaps)++;
                arr[j] = arr[j - h];
                (*comparisons)++;
            }
            if (j >= 0) {
            (*swaps)++;
            }
            arr[j] = temp;
            (*swaps)++;
        }
        h = (h - 1) / 2;
        //(*arr[j])=h;
    }
}

void ShellSortE(int arr[], int n, int *comparisons, int *swaps) {
    int h = ehponenta(n);
    *comparisons = 0;
    *swaps = 0;
    while (h >= 1) {
        for (int i = h; i < n; i++) {
            (*swaps)++;
            int temp = arr[i];
            int j;
            for (j = i; j >= h && arr[j - h] > temp; j -= h) {
                (*swaps)++;
                arr[j] = arr[j - h];
                (*comparisons)++;
            }
            if (j >= 0) {
                (*swaps)++;
                }
            arr[j] = temp;
            (*swaps)++;
        }
        h = (h - 1) / expon;
        //(*arr[j])=h;
    }
}

void IncertSort(int arr[], int n, int *comparisons, int *swaps) {
    *comparisons = 0;
    *swaps = 0;
    
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        (*comparisons)++;
        while (j >= 0 && arr[j] < key) {
            arr[j + 1] = arr[j];
            (*comparisons)++;
            j--;
            (*swaps)++;
        }
        if (j >= 0) {
            (*comparisons)++;
        }
        (*swaps)++;
        arr[j + 1] = key;
    }
}


int main() {
    int n = 100;
    int A[n];

    printf("Для массива из %d элементов:\n", n);

    FillRand(A, n);
    printf("\nСлучайный массив: \n");
    int comparisons = 0, swaps = 0;
    ShellSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    FillRand(A, n);
    printf("Время: %.5f\n", Timer(ShellSort, A, n));

    printf("\nВозрастающий массив: \n");
    FillInc(A, n);
    ShellSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    FillInc(A, n);
    printf("Время: %.5f\n", Timer(ShellSort, A, n));

    printf("\nУбывающий массив: \n");
    FillDec(A, n);
    ShellSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    FillDec(A, n);
    printf("Время: %.5f\n", Timer(ShellSort, A, n));

    //comparisom of methods
    //printf("\n");
    printf("                         ---Shell Sort---\n");
    printf(" n                     h1 ... hn                 R Shell\n");
    for (int n = 1; n < 6; n++) {
        printf("%d|   ", n*100);
        int df = 2;
        int v = 10;        
        int harr[v];
        for (int i=0; i<v; i++){
            harr[i]=0;
        }
        //подсчет кнута
        int h = KnuthStep(n*100);
        int i = 0;
        while ((h >= 1) && (i <= v)) {
            harr[i] = h;
            h = (h-1)/2;
            i++;
        }
        
        int A[n*100];
        int comparisons1 = 0, swaps1 = 0;
        
        FillRand(A, n*100);
        ShellSort(A, n*100, &comparisons1, &swaps1);
        int R = comparisons1 + swaps1;
        
        PrintMas(harr, v);
        printf("|    ");
        printf("%-5d|", R);
        

        printf("\n");
    }
    
    printf("\n");
    printf("                    ---Shell and Insert Sort---\n");
    printf(" n                     h1 ... hn                 R Shell |  R Incert\n");
    for (int n = 1; n < 6; n++) {
        printf("%d|   ", n*100);
        int df = 2;
        int v = 10;        
        int harr[v];
        for (int i=0; i<v; i++){
            harr[i]=0;
        }
        //подсчет кнута
        int h = KnuthStep(n*100);
        int i = 0;
        while ((h >= 1) && (i <= v)) {
            harr[i] = h;
            h = (h-1)/2;
            i++;
        }
        
        int A[n*100];
        int comparisons1 = 0, swaps1 = 0;
        
        FillRand(A, n*100);
        ShellSort(A, n*100, &comparisons1, &swaps1);
        int R = comparisons1 + swaps1;
        
        PrintMas(harr, v);
        printf("|    ");
        printf("%-5d|", R);
        
        //incert
        FillRand(A, n*100);
        IncertSort(A, n*100, &comparisons1, &swaps1);
        R = comparisons1 + swaps1;
        printf("    %-6d|", R);
        printf("\n");
    }
    
    
    printf("\n\n");
    printf("                   ---Shell Sort with exponent---\n");
    printf(" n                     h1 ... hn                 R ShellE   R Shell\n");
    //эксперименты с экспонентой
    for (int n = 1; n < 6; n++) {
        printf("%d|   ", n*100);
        int df = 2;
        int v = 10;        
        int harr[v];
        for (int i=0; i<v; i++){
            harr[i]=0;
        }
        //подсчет некнута
        int h = ehponenta(n*100);
        int i = 0;
        while ((h >= 1) && (i <= v)) {
            harr[i] = h;
            h = (h-1)/expon;
            i++;
        }
        
        int A[n*100];
        int comparisons1 = 0, swaps1 = 0;
        
        FillRand(A, n*100);
        ShellSort(A, n*100, &comparisons1, &swaps1);
        int R1 = comparisons1 + swaps1;
        
        FillRand(A, n*100);
        ShellSortE(A, n*100, &comparisons1, &swaps1);
        int R = comparisons1 + swaps1;
        
        PrintMas(harr, v);
        printf("|    ");
        printf("%-5d", R);
        printf("|    ");
        printf("%-5d|", R1);
        

        printf("\n");
    }


    
    return 0;
}