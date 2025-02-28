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

void ShakerSort(int arr[], int n, int *Mf, int *Sf) {
    int left = 0;
    int right = n - 1;
    int swapped;

    *Mf = 0; // пересылок
    *Sf = 0; // сравнений

    do {
        swapped = 0;

        // Проход слева направо
        for (int i = left; i < right; i++) {
            (*Sf)++;
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
                (*Mf) += 3;
            }
        }
        right--;

        if (!swapped) break; // Если не было пересылок, массив отсортирован

        swapped = 0;

        // Проход справа налево
        for (int i = right; i > left; i--) {
            (*Sf)++;
            if (arr[i] < arr[i - 1]) {
                int temp = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = temp;
                swapped = 1;
                (*Mf) += 3;
            }
        }
        left++;

    } while (swapped);
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

void SelectSort(int arr[], int n, int *comparisons, int *swaps) {
    int i, j, minIdx, temp;
    *comparisons = 0;
    *swaps = 0;

    for (i = 0; i < n - 1; i++) {
        minIdx = i;
        for (j = i + 1; j < n; j++) {
            (*comparisons)++;
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        
        if (minIdx != i) {
            temp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = temp;
            (*swaps)+=3;
        }
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

int SkitfactBubble(int n) {
    int A[n];
    int comparisons1, swaps1;
    
    FillDec(A, n);
    SelectSort(A, n, &comparisons1, &swaps1);
    int D = comparisons1 + swaps1;
    FillRand(A, n);
    SelectSort(A, n, &comparisons1, &swaps1);
    int R = comparisons1 + swaps1;
    FillInc(A, n);
    SelectSort(A, n, &comparisons1, &swaps1);
    int G = comparisons1 + swaps1;
    printf("%d|  ", n);
    printf("%d|  %d|  %d", D, R, G);
}
int SkitfactShaker(int n) {
    int A[n];
    int comparisons1, swaps1;
    
    FillDec(A, n);
    ShakerSort(A, n, &comparisons1, &swaps1);
    int D = comparisons1 + swaps1;
    FillRand(A, n);
    ShakerSort(A, n, &comparisons1, &swaps1);
    int R = comparisons1 + swaps1;
    FillInc(A, n);
    ShakerSort(A, n, &comparisons1, &swaps1);
    int G = comparisons1 + swaps1;
    printf("%d|  %d|  %d", D, R, G);
}

int SkitfactIncert(int n) {
    int A[n];
    int comparisons1, swaps1;
    
    FillDec(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int D = comparisons1 + swaps1;
    FillRand(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int R = comparisons1 + swaps1;
    FillInc(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int G = comparisons1 + swaps1;
    printf("%d|  %d|  %d\n", D, R, G);
}

int SkitfactSelect(int n){
    int A[n];
    int comparisons1, swaps1;
    
    FillDec(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int D = comparisons1 + swaps1;
    FillRand(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int R = comparisons1 + swaps1;
    FillInc(A, n);
    IncertSort(A, n, &comparisons1, &swaps1);
    int G = comparisons1 + swaps1;
    printf("%d|  %d|  %d", D, R, G);
}

void incertprintpretty(int n) {
    for(int i = 1; i <= n; i++){
        int m=i*100;
        int fact=((m*m-m)/4)+m-1+((m*m-m)/4);
        printf("%d|  %d|   ", i*100, fact);
        SkitfactIncert(i * 100);
    }
}

void printallpretty(int n){
    int s=n;
    n=n*100;
    int A[n];
    int comparisons1, swaps1;
    for(int i = 1; i <= s; i++){
        int n=i*100;
        FillRand(A, n);
        SelectSort(A, n, &comparisons1, &swaps1);
        int S = comparisons1 + swaps1;

        FillRand(A, n);
        BubbleSort(A, n, &comparisons1, &swaps1);
        int B = comparisons1 + swaps1;

        FillRand(A, n);
        ShakerSort(A, n, &comparisons1, &swaps1);
        int Sh = comparisons1 + swaps1;

        FillRand(A, n);
        IncertSort(A, n, &comparisons1, &swaps1);
        int I = comparisons1 + swaps1;

        printf("%d|  %d|     %d|     %d|     %d|\n", n, S, B, Sh, I);
    }
}

void MoreSpaces(int n){
    for (int i=0; i<n; i++){
        printf(" ");
    }
}

int main() {
    int n = 100;
    int A[n];

    printf("Для массива из %d элементов:\n", n);

    FillRand(A, n);
    printf("\nСлучайный массив: \n");
    int comparisons = 0, swaps = 0;
    ShakerSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);
    printf("Время: %.5f\n", Timer(ShakerSort, A, n));

    printf("\nВозрастающий массив: \n");
    FillInc(A, n);
    ShakerSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);

    printf("\nУбывающий массив: \n");
    FillDec(A, n);
    ShakerSort(A, n, &swaps, &comparisons);
    PrintMas(A, n);
    printf("Контрольная сумма: %d\n", CheckCum(A, n));
    printf("Серий: %d\n", RunNumber(A, n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число свопов: %d\n", swaps);


    printf("\n\n          Incert Sort\n");
    printf("       F      D      R      G\n");
    incertprintpretty(5);

    printf("\n\n\n          --- COMPARISON TABLE ---\n");
    printf("        S           B        Sh        I\n");
    printallpretty(5);

    printf("\n\n          Bubble sort                      Select sort                        Shaker sort                     Incert sort\n");
    printf("         D      R       G               D      R       G                 D      R       G                D      R       G\n");
    SkitfactBubble(100); MoreSpaces(15); SkitfactSelect(100); MoreSpaces(15); SkitfactShaker(100); MoreSpaces(15); SkitfactIncert(100);
    SkitfactBubble(200); MoreSpaces(12); SkitfactSelect(200); MoreSpaces(14); SkitfactShaker(200); MoreSpaces(14); SkitfactIncert(200);
    SkitfactBubble(300); MoreSpaces(12); SkitfactSelect(300); MoreSpaces(14); SkitfactShaker(300); MoreSpaces(13); SkitfactIncert(300);
    SkitfactBubble(400); MoreSpaces(12); SkitfactSelect(400); MoreSpaces(12); SkitfactShaker(400); MoreSpaces(12); SkitfactIncert(400);
    SkitfactBubble(500); MoreSpaces(9); SkitfactSelect(500); MoreSpaces(11); SkitfactShaker(500); MoreSpaces(12); SkitfactIncert(500);

    return 0;
}

