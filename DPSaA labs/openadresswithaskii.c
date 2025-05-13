#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE_OF_HASH_TABLE 15
#define NUM_OF_STRING 13

typedef struct tData {
    struct tData *next;
    char data;
} tData;

char text[] = 
    "You know, I used to think he and I were the same. Seeing as how he was so small and slender, just like a girl... "
    "In a men\'s society where physical health and strength are the standards we are measured against, he was looked down upon. "
    "I thought that he\'d be frustrated and rebellious, with an inferiority complex simmering under the surface. That he held the same dreams and desires as me, that he was someone who would come running toward the same goal. So I thought I understood his feelings. To my embarrassment, I was mistaken. I thought he would sympathize with and support me, and fight by my side... So when I was admitted to the hospital for my condition, I had him do what I wanted while I was unable to move. And what I wanted was to bring revolution to Yumenosaki and the idol industry. For that purpose, I had to squeeze out the pus and disease and dispose of them. At that time, I was in and out of the hospital, so it was a lot of trouble trying to do everything myself. It was my first trial, as something that no.";

long hash_func(char c) {
    int hash = c % SIZE_OF_HASH_TABLE;
    return hash;
}

void printHashTable(char hash_table[]) {
    printf("\n");
    for (int i = 0; i < SIZE_OF_HASH_TABLE; i++)
        printf("%-6d", i);

    printf("\n");
    for (int i = 0; i < SIZE_OF_HASH_TABLE; i++)
        printf("%-6c", hash_table[i]);
}

int QuadraticProb(char hash_table[], char c, int size_table, int search) {
    int hash = hash_func(c);
    int d = 1;
    int collisions = 0;
    while (1) {
        if (hash_table[hash] == c && search) {
            printf("\nElement is found\n");
            printf("Position: %d\n", hash);
            break;
        }
        if (hash_table[hash] == 0 && !search) {
            hash_table[hash] = c;
            break;
        } else if (!search) {
            collisions++;
        }
        if (d >= size_table && !search) {
            break;
        } else if (d >= size_table && search) {
            printf("\nElement is not found\n");
            break;
        }
        hash = (hash + d * d) % size_table;
        d++;
    }

    return collisions;
}

int LinearProb(char hash_table[], char c, int size_table, int search) {
    int hash = hash_func(c);
    int d = 1;
    int collisions = 0;
    while (1) {
        if (hash_table[hash] == c && search) {
            printf("\nElement is found\n");
            printf("Position: %d\n", hash);
            break;
        }
        if (hash_table[hash] == 0 && !search) {
            hash_table[hash] = c;
            break;
        } else if (!search) {
            collisions++;
        }
        if (d >= size_table && !search) {
            break;
        } else if (d >= size_table && search) {
            printf("\nElement is not found\n");
            break;
        }
        hash = (hash + 1) % size_table;
        d++;
    }

    return collisions;
}

void AddToStack(tData** head, char data) {
    tData* p = (tData*)malloc(sizeof(tData));
    p->data = data;
    p->next = *head;
    *head = p;
}

void fillIncStack(tData **head, int n) {
    for (int i = n; i >= 1; i--) {
        AddToStack(head, (char)(i % 128));
    }
}

void fillDecStack(tData** head, int n) {
    for (int i = 1; i <= n; i++) {
        AddToStack(head, (char)(i % 128));
    }
}

void fillRanStack(tData** head, int n) {
    srand(time(NULL));
    for (int i = 1; i <= n; i++) {
        char ranChar = (char)(rand() % 128);
        AddToStack(head, ranChar);
    }
}

void AddToQueue(tData** head, tData** tail, char data) {
    tData *p = (tData *)malloc(sizeof(tData));
    p->data = data;
    p->next = NULL;

    if (*tail == NULL) {
        *head = *tail = p;
    }
    else {
        (*tail)->next = p;
        *tail = p;
    }
}

void fillIncQueue(tData** head, tData** tail, int n) {
    for (int i = 1; i <= n; i++) {
        AddToQueue(head, tail, (char)(i % 128));
    }
}

void fillDecQueue(tData** head, tData** tail, int n) {
    for (int i = n; i >= 1; i--) {
        AddToQueue(head, tail, (char)(i % 128));
    }
}

void fillRanQueue(tData** head, tData** tail, int n) {
    srand(time(NULL));
    for (int i = 1; i <= n; i++) {
        char ranChar = (char)(rand() % 128);
        AddToQueue(head, tail, ranChar);
    }
}

void delStackElement(tData** head) {
    if (head != NULL) {
        tData *p = *head;
        *head = p->next;
        free(p);
    }
}

void print_list(tData *head) {
    tData *p = head;
    while (p != NULL) {
        printf("%c ", p->data);
        p = p->next;
    }
    printf("\n");
}

int checksum(tData *head) {
    int sum = 0;
    tData *p = head;
    while (p != NULL) {
        sum += p->data;
        p = p->next;
    }
    return sum;
}

int count_series(tData *head) {
    if (head == NULL) {
        return 0;
    }

    int count = 1;
    tData *p = head;

    while (p->next != NULL) {
        if (p->data > p->next->data) {
            count++;
        }
        p = p->next;
    }
    return count;
}

void clear(tData *head) {
    tData *p = head;
    while (p != NULL) {
        tData *temp_p = p;
        p = p->next;
        free(temp_p);
    }
    printf("\n");
}

void printForwardRecursive(tData *head) {
    if (head == NULL) {
        printf("\n");
        return;
    }
    printf("%c ", head->data);
    printForwardRecursive(head->next);
}

void printBackwardRecursive(tData *head) {
    if (head == NULL) {
        return;
    }
    printBackwardRecursive(head->next);
    printf("%c ", head->data);
}

int main() {
    int collisions = 0;
    srand(time(NULL));
    char hashTableLinear[SIZE_OF_HASH_TABLE] = {0};
    char hashTableQuadratic[SIZE_OF_HASH_TABLE] = {0};
    
    printf("Hash table for linear and quadratic prob\n");
    printf("\nLinear prob");
    for (int i = 0; i < NUM_OF_STRING; i++)
        collisions += LinearProb(hashTableLinear, text[rand() % strlen(text)], SIZE_OF_HASH_TABLE, 0);

    printHashTable(hashTableLinear);
    printf("\nCollisions number: %d\n", collisions);

    printf("\n\nQuadratic prob");

    collisions = 0;
    for (int i = 0; i < NUM_OF_STRING; i++)
        collisions += QuadraticProb(hashTableQuadratic, text[rand() % strlen(text)], SIZE_OF_HASH_TABLE, 0);

    printHashTable(hashTableQuadratic);
    printf("\nCollisions number: %d\n", collisions);

    printf("\n\nCollisions");
    int simple_num[5] = {11, 23, 37, 47, 53};
    printf("\n\nSize    Count   Linear  Quadratic\n");

    for (int i = 0; i < 5; i++) {
        int line_colis = 0;
        int quadr_colis = 0;
        char hashTableLinearTemp[simple_num[i]];
        char hashTableQuadraticTemp[simple_num[i]];
        
        for (int j = 0; j < simple_num[i]; j++) {
            hashTableLinearTemp[j] = 0;
            hashTableQuadraticTemp[j] = 0;
        }
        
        int count = NUM_OF_STRING;
        for (int j = 0; j < count; j++)
            line_colis += LinearProb(hashTableLinearTemp, text[rand() % strlen(text)], simple_num[i], 0);

        for (int j = 0; j < count; j++)
            quadr_colis += QuadraticProb(hashTableQuadraticTemp, text[rand() % strlen(text)], simple_num[i], 0);

        printf("%d\t\t%d\t\t%d\t\t%d\n", simple_num[i], count, line_colis, quadr_colis);
    }

    printf("\n\nSearch hash table\n");
    char hashTableLinearHash[SIZE_OF_HASH_TABLE] = {0};
    char hashTableQuadraticHash[SIZE_OF_HASH_TABLE] = {0};
    printf("\nLinear prob");
    for (int i = 0; i < NUM_OF_STRING; i++)
        LinearProb(hashTableLinearHash, text[rand() % strlen(text)], SIZE_OF_HASH_TABLE, 0);

    printHashTable(hashTableLinearHash);

    printf("\n\nQuadratic prob");

    for (int i = 0; i < NUM_OF_STRING; i++)
        QuadraticProb(hashTableQuadraticHash, text[rand() % strlen(text)], SIZE_OF_HASH_TABLE, 0);

    printHashTable(hashTableQuadraticHash);

    int choose = 0;
    printf("\n\nChoose search method. 0 - Linear, 1 - Quadratic: ");
    scanf("%d", &choose);
    printf("Input character (0 to exit): ");

    while (1) {
        char c;
        scanf(" %c", &c);
        if (c == '0') break;

        if (choose == 0)
            LinearProb(hashTableLinearHash, c, SIZE_OF_HASH_TABLE, 1);
        if (choose == 1)
            QuadraticProb(hashTableQuadraticHash, c, SIZE_OF_HASH_TABLE, 1);
        
        printf("\n\nChoose search method. 0 - Linear, 1 - Quadratic: ");
        scanf("%d", &choose);
        printf("Input character (0 to exit): ");
    }

    return 0;
}