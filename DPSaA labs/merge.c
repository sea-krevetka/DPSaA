#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
} Queue;

void initQueue(Queue* q) {
    q->head = q->tail = NULL;
}

void enqueue(Queue* q, int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    
    if (q->tail == NULL) {
        q->head = q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
}

int dequeue(Queue* q) {
    if (q->head == NULL) return -1;
    
    Node* temp = q->head;
    int data = temp->data;
    q->head = q->head->next;
    
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(temp);
    return data;
}

int isQueueEmpty(Queue* q) {
    return q->head == NULL;
}

void splitList(Node* source, Node** a, Node** b, int* n) {
    *n = 0;
    if (source == NULL) return;
    
    Node* current = source;
    *a = current;
    *b = current->next;
    
    Node* lastA = *a;
    Node* lastB = *b;
    
    current = (current->next != NULL) ? current->next->next : NULL;
    *n = 2;
    
    while (current != NULL) {
        lastA->next = current;
        lastA = lastA->next;
        (*n)++;
        
        if (current->next != NULL) {
            lastB->next = current->next;
            lastB = lastB->next;
            (*n)++;
            current = current->next->next;
        } else {
            current = current->next;
        }
    }
    
    if (lastA != NULL) lastA->next = NULL;
    if (lastB != NULL) lastB->next = NULL;
}

void merge(Node** a, int q, Node** b, int r, Queue* c, int* cf, int* mf) {
    *cf = 0;
    *mf = 0;
    
    while (q > 0 && r > 0) {
        (*cf)++;
        if ((*a)->data <= (*b)->data) {
            enqueue(c, (*a)->data);
            *a = (*a)->next;
            q--;
        } else {
            enqueue(c, (*b)->data);
            *b = (*b)->next;
            r--;
        }
        (*mf)++;
    }
    
    while (q > 0) {
        enqueue(c, (*a)->data);
        *a = (*a)->next;
        q--;
        (*mf)++;
    }
    
    while (r > 0) {
        enqueue(c, (*b)->data);
        *b = (*b)->next;
        r--;
        (*mf)++;
    }
}

void mergeSort(Node** S, int* totalCf, int* totalMf) {
    *totalCf = 0;
    *totalMf = 0;
    
    if (*S == NULL || (*S)->next == NULL) return;
    
    int n;
    Node* a;
    Node* b;
    splitList(*S, &a, &b, &n);
    
    int p = 1;
    Queue c[2];
    int i, m, q, r;
    int cf, mf;
    
    while (p < n) {
        initQueue(&c[0]);
        initQueue(&c[1]);
        i = 0;
        m = n;
        
        while (m > 0) {
            q = (m >= p) ? p : m;
            m -= q;
            
            r = (m >= p) ? p : m;
            m -= r;
            
            merge(&a, q, &b, r, &c[i], &cf, &mf);
            *totalCf += cf;
            *totalMf += mf;
            
            i = 1 - i;
        }
        
        Node* tailA = NULL;
        Node* tailB = NULL;
        a = NULL;
        b = NULL;
        
        while (!isQueueEmpty(&c[0])) {
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->data = dequeue(&c[0]);
            newNode->next = NULL;
            
            if (a == NULL) {
                a = tailA = newNode;
            } else {
                tailA->next = newNode;
                tailA = newNode;
            }
            (*totalMf)++;
        }
        
        while (!isQueueEmpty(&c[1])) {
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->data = dequeue(&c[1]);
            newNode->next = NULL;
            
            if (b == NULL) {
                b = tailB = newNode;
            } else {
                tailB->next = newNode;
                tailB = newNode;
            }
            (*totalMf)++;
        }
        
        p *= 2;
    }
    
    Node* current = a;
    Node* prev = NULL;
    while (current != NULL) {
        prev = current;
        current = current->next;
    }
    if (prev != NULL) {
        prev->next = b;
    } else {
        a = b;
    }
    
    *S = a;
}

void printList(Node* node) {
    while (node != NULL) {
        printf("%d ", node->data);
        node = node->next;
    }
    printf("\n");
}

void push(Node** head_ref, int new_data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = new_data;
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}

void freeList(Node* head) {
    Node* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main() {
    Node* head = NULL;
    
    int elements[] = {12, 3, 7, 9, 14, 6, 11, 2, 17, 5, 19, 8, 1, 15, 4, 10, 16, 13, 20, 18};
    for (int i = 0; i < 20; i++) {
        push(&head, elements[i]);
    }
    
    printf("Исходный список:\n");
    printList(head);
    
    int totalCf, totalMf;
    mergeSort(&head, &totalCf, &totalMf);
    
    printf("Отсортированный список:\n");
    printList(head);
    
    printf("N     ");
    
    int n = 20;
    int C = n * (int)(log(n) / log(2)); // n*log2(n)
    int M = n * (int)(log(n) / log(2)) + n;
    
    printf("Теоретическая оценка сравнений (C): %d\n", C);
    printf("Теоретическая оценка перемещений (M): %d\n", M);

    printf("\nТрудоемкость Mergesort\n");
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
    
    freeList(head);
    return 0;
}