#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void FillMatrix(int n, int **Mat) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Mat[i][j] = 1;
        }
    }
}

void PrintMatrix(int n, int **Mat) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", Mat[i][j]);
        }
        printf("\n");
    }
}

typedef struct {
    int n;
    int start_row;
    int end_row;
    int **A;
    int **B;
    int **C;
} matrix_thread_data;

void *multiply_thread(void *arg) {
    matrix_thread_data *data = (matrix_thread_data *)arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->n; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->n; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    return NULL;
}

int main(void) {
    // Переменные для изменения параметров
    int n_start = 100, n_end = 2500, n_step = 500; // Диапазон и шаг для размера матрицы
    int thread_start = 1, thread_end = 128, thread_step = 2; // Диапазон и шаг для количества потоков

    // Время выполнения
    clock_t start_time, end_time;
    
    for (int n = n_start; n <= n_end; n += n_step) {
        for (int matrix_threads = thread_start; matrix_threads <= thread_end; matrix_threads *= thread_step) {
            // Выделение памяти для матриц
            int **A = malloc(n * sizeof(int *));
            int **B = malloc(n * sizeof(int *));
            int **C = malloc(n * sizeof(int *));
            for (int i = 0; i < n; i++) {
                A[i] = malloc(n * sizeof(int));
                B[i] = malloc(n * sizeof(int));
                C[i] = malloc(n * sizeof(int));
            }

            FillMatrix(n, A);
            FillMatrix(n, B);

            pthread_t threads[matrix_threads];
            matrix_thread_data thread_data[matrix_threads];

            int rows_per_thread = n / matrix_threads;
            int remaining_rows = n % matrix_threads;
            int current_row = 0;

            // Измерение времени выполнения
            start_time = clock();

            // Создание потоков
            for (int i = 0; i < matrix_threads; i++) {
                thread_data[i].n = n;
                thread_data[i].A = A;
                thread_data[i].B = B;
                thread_data[i].C = C;

                int rows_for_this_thread = rows_per_thread + (i < remaining_rows ? 1 : 0);

                thread_data[i].start_row = current_row;
                thread_data[i].end_row = current_row + rows_for_this_thread;
                current_row += rows_for_this_thread;

                pthread_create(&threads[i], NULL, multiply_thread, &thread_data[i]);
            }

            // Ожидание завершения работы всех потоков
            for (int i = 0; i < matrix_threads; i++) {
                pthread_join(threads[i], NULL);
            }

            // Измерение времени выполнения
            end_time = clock();

            double execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            printf("Размер матрицы: %d, Количество потоков: %d, Время выполнения: %.6f сек\n", n, matrix_threads, execution_time);

            // Освобождение памяти
            for (int i = 0; i < n; i++) {
                free(A[i]);
                free(B[i]);
                free(C[i]);
            }
            free(A);
            free(B);
            free(C);
        }
    }

    return 0;
}
