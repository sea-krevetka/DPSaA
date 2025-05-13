#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int C = 0;
int M = 0;


void FillInc(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }
}

void FillDec(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = n - i;
    }
}

void FillRand(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 1000;
    }
}

int CheckSum(int n, int A[]) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += A[i];
    }
    printf("\nКонтрольная сумма: %d\n\n",sum);
    return sum;
}

void PrintMas(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        printf("%d ", A[i]);
    }
    printf("");
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    M += 3;
}

void QuickSort(int A[], int L, int R) {
    if (L >= R) return;
    
    int x = A[L];
    int i = L;
    int j = R;
    
    while (i <= j) {

        while (A[i] < x) {
            C++;
            i++;
        }
        C++;
        
        while (A[j] > x) {
            C++;
            j--;
        }
        C++;
        
        if (i <= j) {
            swap(&A[i], &A[j]);
            i++;
            j--;
        }
    }
    
    if (L < j) QuickSort(A, L, j);
    if (i < R) QuickSort(A, i, R);
}
int main() {
    
    int n = 100;
    int A[n];
    printf("Для массива из %d элементов:\n", n);
    FillRand(n, A);
    printf("\nСлучайный массив: \n");
    QuickSort(A, 0, n-1);
    PrintMas(n,A);
    CheckSum(n, A);
    
    FillDec(n, A);
    printf("Убывающий массив: \n");
    QuickSort(A, 0, n-1);
    PrintMas(n,A);
    CheckSum(n, A);
    
    FillInc(n, A);
    printf("Возрастающий массив: \n");
    QuickSort(A, 0, n-1);
    PrintMas(n,A);
    CheckSum(n, A);



    printf("\nТрудоемкость QuickSort\n");
    printf("      N  |             D |             R |             G |\n");
    
    for (int i = 1; i < 6; i++) {
        int n = 100 * i;
        int A_dec[n], A_rand[n], A_inc[n];

        FillDec(n, A_dec);
        FillRand(n, A_rand);
        FillInc(n, A_inc);
        
        C = 0; M = 0;
        QuickSort(A_dec, 0, n-1);
        int T_dec = C + M;
        
        
        C = 0; M = 0;
        QuickSort(A_rand, 0, n-1);
        int T_rand = C + M;

        
        C = 0; M = 0;
        QuickSort(A_inc, 0, n-1);
        int T_inc = C + M;
        
        printf(" %6d  | %13d | %13d | %13d |\n", n, T_dec, T_rand, T_inc);
    }
    return 0;
}