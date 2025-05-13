#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CELLS 256

typedef struct tData {
    struct tData *next;
    union {
        int Data;
        unsigned char digit[sizeof(int)];
    } data;
} tData;

typedef struct {
    tData *head;
    tData *tail;
} tQueue;

int M = 0;

void AddToQueue(tData **head, tData **tail, int data) {
    tData *p = (tData *)malloc(sizeof(tData));
    p->data.Data = data;
    p->next = NULL;
    if (*tail == NULL) {
        *head = *tail = p;
    } else {
        (*tail)->next = p;
        *tail = p;
    }
}

void fillDecQueue(tData **head, tData **tail, int n) {
    for (int i = n; i >= 1; i--) {
        AddToQueue(head, tail, i);
    }
}

void fillRanQueue(tData **head, tData **tail, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int ranNum = rand() % 1000 + 1;
        AddToQueue(head, tail, ranNum);
    }
}

void fillIncQueue(tData **head, tData **tail, int n) {
    for (int i = 1; i <= n; i++) {
        AddToQueue(head, tail, i);
    }
}

void print_list(tData *head) {
    while (head != NULL) {
        printf("%d ", head->data.Data);
        head = head->next;
    }
    printf("\n");
}

void clear(tData *head) {
    while (head != NULL) {
        tData *temp = head;
        head = head->next;
        free(temp);
    }
}

int checksum(tData *head) {
    int sum = 0;
    while (head != NULL) {
        sum += head->data.Data;
        head = head->next;
    }
    return sum;
}

int count_series(tData *head) {
    if (!head) return 0;
    int count = 1;
    int prev = head->data.Data;
    tData *current = head->next;
    while (current) {
        if (current->data.Data < prev) count++;
        prev = current->data.Data;
        current = current->next;
    }
    return count;
}

tData* DigitalSort(tData *head, int bytes) {
    tData* cells[CELLS] = {0};
    tData* cellsTails[CELLS] = {0};

    for (int bit = 0; bit < bytes * 8; bit++) {
        tData *current = head;

        for (int i = 0; i < CELLS; i++) {
            cells[i] = NULL;
            cellsTails[i] = NULL;
        }

        while (current) {
            int digit = (current->data.Data >> bit) & 0x1;
            tData *next = current->next;
            current->next = NULL;

            if (!cells[digit]) {
                cells[digit] = cellsTails[digit] = current;
            } else {
                cellsTails[digit]->next = current;
                cellsTails[digit] = current;
            }

            current = next;
        }

        head = NULL;
        tData **tail = &head;

        for (int i = 0; i < 2; i++) {
            tData *bucket = cells[i];
            while (bucket) {
                *tail = bucket;
                tail = &bucket->next;
                bucket = bucket->next;
                M++;
            }
        }
    }

    return head;
}

void testDigitalSort(int bytes) {
    int sizes[] = {100, 200, 300, 400, 500};
    printf("\nТестирование цифровой сортировки %d бит:\n", bytes * 8);
    printf("  N     |    Teor     |    D    |    R    |    G    |\n");

    for (int i = 0; i < 5; i++) {
        int n = sizes[i];
        int theory = bytes * 8 * n;
        int results[3] = {0};

        for (int t = 0; t < 3; t++) {
            tData *head = NULL, *tail = NULL;

            switch(t) {
                case 0: fillDecQueue(&head, &tail, n); break;
                case 1: fillRanQueue(&head, &tail, n); break;
                case 2: fillIncQueue(&head, &tail, n); break;
            }

            M = 0;
            head = DigitalSort(head, bytes);
            results[t] = M;
            clear(head);
        }
        printf("| %-5d | %-11d | %-7d | %-7d | %-7d |\n", n, theory, results[0], results[1], results[2]);
    }
}

void demoSort(int bytes, int n) {
    tData *head = NULL, *tail = NULL;
    fillRanQueue(&head, &tail, n);

    printf("\n""Демонстрация работы сортировки (%d элементов):\n", n);
    printf("Исходный список:\n");
    print_list(head);

    int sum_before = checksum(head);
    int series_before = count_series(head);
    printf("Контрольная сумма до сортировки: %d\n", sum_before);
    printf("Количество серий до сортировки: %d\n", series_before);

    M = 0;
    head = DigitalSort(head, bytes);

    printf("\n" "Отсортированный список (M=%d):\n", M);
    print_list(head);

    int sum_after = checksum(head);
    int series_after = count_series(head);
    printf("Контрольная сумма после сортировки: %d\n", sum_after);
    printf("Количество серий после сортировки: %d\n", series_after);

    clear(head);
}
int main() {
    testDigitalSort(4);
    demoSort(4, 20);

    testDigitalSort(2);
    demoSort(2, 20);

    return 0;
}