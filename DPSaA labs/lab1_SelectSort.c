#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void FillInc(int A[], int n) {
    for (int i = 0; i < n; i++){
    A[i] = i+1;}
}
void FillDec(int A[], int n) {
    for (int i = 0; i < n; i++){
    A[i] = n - i;}
}
void FillRand(int A[], int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++){
    A[i] = rand() % 100;}
}
int CheckSum(int A[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++){
    sum += A[i];}
    return sum;
}
int RunNumber(int A[], int n){
    int runs = 1;
    for (int i = 1; i < n; i++){
            if (A[i] < A[i-1]) runs++;
    }
    return runs;
}
void PrintMas(int A[], int n){
    for (int i = 0; i < n; i++){
    printf("%d ", A[i]);}
    printf("\n");
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


void SelectSort1(int arr[], int n, int *comparisons, int *swaps) {
    int i, j, minIdx, temp;
    *comparisons = 0;
    *swaps = 0; 

    for (i = 0; i < n - 1; i++) {
        minIdx = i; 
        for (j = i + 1; j < n; j++) {
            (*comparisons)++; 
            if (arr[j] <= arr[minIdx]) { 
                minIdx = j;
            }
        }
        
        temp = arr[i];
        arr[i] = arr[minIdx];
        arr[minIdx] = temp;
        (*swaps) += 3;
    }
}




int main() {

    int n=10;
    int A[n];
    int comparisons, swaps;

    int M = 3*(n - 1);
    int S = n * (n - 1) / 2;

    int M10=M;
    int S10=S;

    printf("Для массива из 10 элементов:\n");

    printf("Теоретическое количество пересылок: %d\n", M10);
    printf("Теоретическое количество сравнений: %d\n", S10);

    FillRand(A,n);
    printf("\nСлучайный массив: \n");
    SelectSort(A, n, &comparisons, &swaps);
    PrintMas(A,n);
    printf("Контрольная сумма: %d\n", CheckSum(A,n));
    printf("Серий: %d\n", RunNumber(A,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Rcomp=comparisons;
    int Rswaps=swaps;

    printf("\n");

    FillInc(A,n);
    printf("Возрастающий массив: \n");
    SelectSort(A, n, &comparisons, &swaps);
    PrintMas(A,n);
    printf("Контрольная сумма: %d\n", CheckSum(A,n));
    printf("Серий: %d\n", RunNumber(A,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Gcomp=comparisons;
    int Gswaps=swaps;

    printf("\n");

    FillDec(A,n);
    printf("\nУбывающий массив: \n");
    SelectSort(A, n, &comparisons, &swaps);
    PrintMas(A,n);
    printf("Контрольная сумма: %d\n", CheckSum(A,n));
    printf("Серий: %d\n", RunNumber(A,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Dcomp=comparisons;
    int Dswaps=swaps;

    printf("\n\n\nДля массива из 100 элементов:\n");

    //дополнительно для таблицы
    n=100;
    int B[n];

    M = 3*(n - 1);
    S = n * (n - 1) / 2;

    int M100=M;
    int S100=S;

    printf("Теоретическое количество пересылок: %d\n", M100);
    printf("Теоретическое количество сравнений: %d\n", S100);

    FillRand(B,n);
    printf("\nСлучайный массив: \n");
    SelectSort(B, n, &comparisons, &swaps);
    PrintMas(B,n);
    printf("Контрольная сумма: %d\n", CheckSum(B,n));
    printf("Серий: %d\n", RunNumber(B,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Rcomp1=comparisons;
    int Rswaps1=swaps;

    printf("\n");

    FillInc(B,n);
    printf("Возрастающий массив: \n");
    SelectSort(B, n, &comparisons, &swaps);
    PrintMas(B,n);
    printf("Контрольная сумма: %d\n", CheckSum(B,n));
    printf("Серий: %d\n", RunNumber(B,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Gcomp1=comparisons;
    int Gswaps1=swaps;

    printf("\n");

    FillDec(B,n);
    printf("\nУбывающий массив: \n");
    SelectSort(B, n, &comparisons, &swaps);
    PrintMas(B,n);
    printf("Контрольная сумма: %d\n", CheckSum(B,n));
    printf("Серий: %d\n", RunNumber(B,n));
    printf("Число сравнений: %d\n", comparisons);
    printf("Число пересылок: %d\n", swaps);
    int Dcomp1=comparisons;
    int Dswaps1=swaps;


    int m=10;
    int C[m];
    FillRand(C,m);
    SelectSort1(C, m, &comparisons, &swaps);
    int I10=comparisons+swaps;
    
    m=100;
    int D[m];
    FillRand(D,m);
    SelectSort1(D, m, &comparisons, &swaps);
    int I100=comparisons+swaps;
    //конец доп кода



    printf("\n\n\n           Трудоемкость прямого выбора: \n");
    printf(" N      М+С теор.       Исходный М+С           Улучшенный М+С\n");
    printf("                          R   G   D              R   G   D\n");
    printf(" 10        %d            %d |%d |%d             %d |%d |%d    \n",M10+S10, I10,I10,I10, Rcomp+Rswaps, Gcomp+Gswaps, Dcomp+Dswaps);
    printf("100       %d         %d |%d |%d       %d |%d |%d    \n",M100+S100,I100,I100,I100, Rcomp1+Rswaps1, Gcomp1+Gswaps1, Dcomp1+Dswaps1);
    return 0;
}
