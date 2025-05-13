#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tData {
    struct tData *next;
    char data;
} tData;

int SizeHashTable = 19;
int collisions = 0;

int hash_func(char c) {
    return c % SizeHashTable;
}

void AddToStack(tData** head, char data) {
    tData* p = (tData*)malloc(sizeof(tData));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    p->data = data;
    p->next = *head;
    *head = p;
}

void printList(tData *head) {
    tData *p = head;
    while (p != NULL) {
        printf(" -> '%c'", p->data);
        p = p->next;
    }
    printf("\n");
}

void printHashTable(tData **hashTable) {
    for (int i = 0; i < SizeHashTable; i++) {
        printf("%d:", i);
        printList(hashTable[i]);
    }
    printf("\n");
}

void addToTheTable(tData **hashTable, char elem, int *collisions) {
    int hash = hash_func(elem);
    tData *p = hashTable[hash];
    
    tData *current = p;
    while (current != NULL) {
        if (current->data == elem) {
            return;
        }
        current = current->next;
    }
    
    AddToStack(&hashTable[hash], elem);
    if (p != NULL) {
        (*collisions)++;
    }
}

void HashSearch(tData **hashTable, char elem) {
    int x = hash_func(elem);
    int index = 1;
    tData *p = hashTable[x];

    while (p != NULL) {
        if (p->data == elem) {
            printf("Element '%c' is found in list %d at position %d\n", elem, x, index);
            return;
        }
        index++;
        p = p->next;
    }
    printf("Element '%c' isn't found\n", elem);
}

void clearHashTable(tData **hashTable, int size) {
    for (int i = 0; i < size; i++) {
        tData *p = hashTable[i];
        while (p != NULL) {
            tData *temp = p;
            p = p->next;
            free(temp);
        }
        hashTable[i] = NULL;
    }
}

int main() {
    char text[] = 
    "You know, I used to think he and I were the same. Seeing as how he was so small and slender, just like a girl... "
    "In a men\'s society where physical health and strength are the standards we are measured against, he was looked down upon. "
    "I thought that he\'d be frustrated and rebellious, with an inferiority complex simmering under the surface. That he held the same dreams and desires as me, that he was someone who would come running toward the same goal. So I thought I understood his feelings. To my embarrassment, I was mistaken. I thought he would sympathize with and support me, and fight by my side... So when I was admitted to the hospital for my condition, I had him do what I wanted while I was unable to move. And what I wanted was to bring revolution to Yumenosaki and the idol industry. For that purpose, I had to squeeze out the pus and disease and dispose of them. At that time, I was in and out of the hospital, so it was a lot of trouble trying to do everything myself. It was my first trial, as something that no.";
    
    // Выводим весь текст
    printf("Full text (%zu bytes):\n", strlen(text));
    printf("--------------------------------------------------\n");
    printf("%s\n", text);
    printf("--------------------------------------------------\n\n");

    tData **hashTable = (tData**)calloc(SizeHashTable, sizeof(tData*));
    if (hashTable == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Добавляем все символы текста в хеш-таблицу
    for (size_t i = 0; i < strlen(text); i++) {
        addToTheTable(hashTable, text[i], &collisions);
    }
    
    printf("Hash table statistics:\n");
    printf("Total characters: %zu\n", strlen(text));
    printf("Hash table size: %d\n", SizeHashTable);
    printf("Collisions count: %d\n\n", collisions);

    // Выводим хеш-таблицу (можно закомментировать, если она слишком большая)
    printf("Hash table contents:\n");
    printHashTable(hashTable);

    // Поиск символа в хеш-таблице
    char searchChar;
    printf("\nInput character for search: ");
    scanf(" %c", &searchChar);  // Пробел перед %c чтобы пропустить предыдущие символы новой строки
    HashSearch(hashTable, searchChar);

    // Очистка памяти
    clearHashTable(hashTable, SizeHashTable);
    free(hashTable);

    return 0;
}