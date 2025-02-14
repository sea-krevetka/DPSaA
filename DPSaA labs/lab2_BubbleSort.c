#include <stdio.h>
#include <stdlib.h>
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

int CheckSum(int A[], int n) {
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
        printf("%d ", A[i]);
    }
    printf("\n");
}

void BubbleSort(int arr[], int n, int *comparisons, int *swaps) {
    int temp;
    *comparisons = 0;
    *swaps = 0;

    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i - 1; j++) {
            (*comparisons)++;
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                (*swaps) += 3;
            }
        }
    }
}

int JOJOtheory(int n) {
    return 3 * (n * (n - 1) / 2) + (n * (n - 1) / 2);
}

int Skitfact(int n) {
    int A[n];
    int comparisons1, swaps1;
    
    FillDec(A, n);
    BubbleSort(A, n, &comparisons1, &swaps1);
    int D = comparisons1 + swaps1;
    FillRand(A, n);
    BubbleSort(A, n, &comparisons1, &swaps1);
    int R = comparisons1 + swaps1;
    FillInc(A, n);
    BubbleSort(A, n, &comparisons1, &swaps1);
    int G = comparisons1 + swaps1;
    
    printf("%d|  %d|  %d\n", D, R, G);
}

double Timer(void (*sortFunc)(int[], int, int*, int*), int A[], int n) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    int comparisons, swaps;
    sortFunc(A, n, &comparisons, &swaps);
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
}

int main() {
    int n = 100;
    int A[n];
    
    printf("Для массива из %d элементов:\n", n);

    FillRand(A, n);
    printf("\nСлучайный массив: \n");
    int comparisons, swaps;
    BubbleSort(A, n, &comparisons, &swaps);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckSum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    printf("Time: %.3f\n", Timer(BubbleSort, A, n));

    printf("\n");

    FillInc(A, n);
    printf("Возрастающий массив: \n");
    BubbleSort(A, n, &comparisons, &swaps);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckSum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    printf("Time: %.3f\n", Timer(BubbleSort, A, n));

    printf("\n");

    FillDec(A, n);
    printf("\nУбывающий массив: \n");
    BubbleSort(A, n, &comparisons, &swaps);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckSum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    printf("Time: %.3f\n", Timer(BubbleSort, A, n));

    printf(" N     M+C теор               M+Сфакт\n");
    printf("                          D     R     G\n");
    printf(" 100     %d           ", JOJOtheory(100)); Skitfact(100);
    printf(" 200     %d           ", JOJOtheory(200)); Skitfact(200);
    printf(" 300     %d         ", JOJOtheory(300)); Skitfact(300);
    printf(" 400     %d         ", JOJOtheory(400)); Skitfact(400);
    printf(" 500     %d         ", JOJOtheory(500)); Skitfact(500);

    return 0;
}