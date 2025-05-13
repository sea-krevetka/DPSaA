#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int C = 0;
int M = 0;

int C_build = 0;
int M_build = 0;

void FillInc(int n, int A[])
{
    for (int i = 0; i < n; i++)
    {
        A[i] = i + 1;
    }
}

void FillDec(int n, int A[])
{
    for (int i = 0; i < n; i++)
    {
        A[i] = n - i;
    }
}

void FillRand(int n, int A[])
{

    for (int i = 0; i < n; i++)
    {
        A[i] = rand() % 1000;
    }
}

int CheckSum(int n, int A[])
{
    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += A[i];
    }
    return sum;
}

void PrintMas(int n, int A[])
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }
    printf("\n");
}

double calculate_M(int n)
{
    return n * log2(n) + 6.5 * n - 4;
}

double calculate_C(int n)
{
    return 2 * n * log2(n) + n + 2;
}

double calculate_M_build(int n)
{
    return log2(n) + 2;
}

double calculate_C_build(int n)
{
    return 2 * log2(n) + 1;
}

void HeapBuild(int A[], int L, int R)
{
    int x = A[L];
    M++;
    M_build++;
    int i = L;
    while (1)
    {
        int j = 2 * i + 1;
        if (j > R)
            break;

        C++;
        C_build++;
        if (j < R && A[j + 1] > A[j])
        {
            j++;
        }

        C++;
        C_build++;
        if (x >= A[j])
            break;

        A[i] = A[j];
        M++;
        M_build++;
        i = j;
    }
    
    if (i != L) {
        M++;
        M_build++;
    }
    M++;
    M_build++;
    A[i] = x;
}

void HeapSort(int A[], int n)
{
    int temp;
    for (int L = n / 2 - 1; L >= 0; L--)
    {
        HeapBuild(A, L, n - 1);
    }

    for (int R = n - 1; R > 0; R--)
    {
        temp = A[0];
        M++;
        A[0] = A[R];
        M++;
        A[R] = temp;
        M++;

        HeapBuild(A, 0, R - 1);
    }
}

int main()
{
    srand(time(NULL));
    printf("Трудоемкость построения пирамиды\n");
    printf("  N    |              T|              R|              G|              D|\n");
    
    for (int i = 1; i < 6; i++)
    {
        int n = 100 * i;
        int A[n];
        int M_teor_build = calculate_M_build(n);
        int C_teor_build = calculate_C_build(n);
        int teor_T_build = M_teor_build + C_teor_build;

        FillDec(n, A);
        HeapBuild(A, 0, n-1);
        int T_buildDec = C_build + M_build;
        C_build=0;
        M_build=0;

        FillRand(n, A);
        HeapBuild(A, 0, n-1);
        int T_buildRan = C_build + M_build;
        C_build=0;
        M_build=0;

        FillInc(n, A);
        HeapBuild(A, 0, n-1);
        int T_buildInc = C_build + M_build;
        C_build=0;
        M_build=0;

        printf(" %4d  | %13d | %13d | %13d | %13d |\n", 
               n, teor_T_build, T_buildDec, T_buildRan, T_buildInc);
    }

    printf("\n\nТрудоемкость пирамидальной сортировки\n");
    printf("  N    |              D|              R|             G |\n");

    for (int i = 1; i < 6; i++)
    {
        int n = 100 * i;
        int A[n];
        int M_teor = calculate_M(n);
        int C_teor = calculate_C(n);
        int teor_T = M_teor + C_teor;

        FillDec(n, A);
        HeapSort(A, n);
        int decT = C + M;
        C = 0;
        M = 0;
        FillRand(n, A);
        HeapSort(A, n);
        int randT = C + M;
        C = 0;
        M = 0;

        FillInc(n, A);
        HeapSort(A, n);
        int incT = C + M;
        C = 0;
        M = 0;

        printf(" %4d  | %13d | %13d | %13d |\n", n, decT, randT, incT);
    }

    return 0;
}