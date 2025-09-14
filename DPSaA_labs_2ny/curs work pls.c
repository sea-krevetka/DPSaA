#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PAGE_SIZE 10

typedef struct Citizen {
    char full_name[32];
    char street[18];
    short int house;
    short int apartment;
    char settlement_date[10];
    struct Citizen* next;
} Citizen;

typedef struct Node {
    Citizen* data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
} Queue;

void initQueue(Queue* q) {
    q->head = q->tail = NULL;
}

void enqueue(Queue* q, Citizen* data) {
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

Citizen* dequeue(Queue* q) {
    if (q->head == NULL) return NULL;
    
    Node* temp = q->head;
    Citizen* data = temp->data;
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

int compareCitizens(Citizen* a, Citizen* b) {
    int year_cmp = strcmp(a->settlement_date + 6, b->settlement_date + 6);
    if (year_cmp != 0) return year_cmp;
    
    return strcmp(a->street, b->street);
}

void splitList(Citizen* source, Citizen** a, Citizen** b, int* n) {
    *n = 0;
    if (source == NULL) return;
    
    Citizen* slow = source;
    Citizen* fast = source->next;
    
    // Используем алгоритм "черепахи и зайца" для разделения списка
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    
    *a = source;
    *b = slow->next;
    slow->next = NULL;
    
    // Подсчет элементов
    Citizen* current = source;
    *n = 0;
    while (current != NULL) {
        (*n)++;
        current = current->next;
    }
}

void merge(Citizen** a, int q, Citizen** b, int r, Queue* c) {
    while (q > 0 && r > 0) {
        if (compareCitizens(*a, *b) <= 0) {
            enqueue(c, *a);
            *a = (*a)->next;
            q--;
        } else {
            enqueue(c, *b);
            *b = (*b)->next;
            r--;
        }
    }
    
    while (q > 0) {
        enqueue(c, *a);
        *a = (*a)->next;
        q--;
    }
    
    while (r > 0) {
        enqueue(c, *b);
        *b = (*b)->next;
        r--;
    }
}

void mergeSort(Citizen** S) {
    if (*S == NULL || (*S)->next == NULL) return;
    
    int n;
    Citizen* a;
    Citizen* b;
    splitList(*S, &a, &b, &n);
    
    mergeSort(&a);
    mergeSort(&b);
    
    Queue c;
    initQueue(&c);
    merge(&a, n/2 + n%2, &b, n/2, &c);
    
    // Восстанавливаем отсортированный список из очереди
    *S = dequeue(&c);
    Citizen* current = *S;
    while (!isQueueEmpty(&c)) {
        current->next = dequeue(&c);
        current = current->next;
    }
    current->next = NULL;
}

Citizen** createIndexArray(Citizen* head, int size) {
    Citizen** index = (Citizen**)malloc(size * sizeof(Citizen*));
    Citizen* current = head;
    
    for (int i = 0; i < size && current != NULL; i++) {
        index[i] = current;
        current = current->next;
    }
    
    return index;
}

Citizen** searchByYear(Citizen** index, int size, const char* year, int* result_size) {
    Citizen** results = NULL;
    *result_size = 0;
    
    for (int i = 0; i < size; i++) {
        if (strlen(index[i]->settlement_date) >= 8 && 
            strcmp(index[i]->settlement_date + 6, year) == 0) {
            (*result_size)++;
            results = (Citizen**)realloc(results, (*result_size) * sizeof(Citizen*));
            results[(*result_size) - 1] = index[i];
        }
    }
    
    return results;
}

void printCitizen(Citizen* citizen) {
    printf("%-32s %-18s %-6hd %-10hd %-10s\n", 
           citizen->full_name, 
           citizen->street, 
           citizen->house, 
           citizen->apartment, 
           citizen->settlement_date);
}

Citizen* loadFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Ошибка открытия файла %s\n", filename);
        return NULL;
    }
    
    Citizen* head = NULL;
    Citizen* tail = NULL;
    *count = 0;
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    int record_size = sizeof(char[32]) + sizeof(char[18]) + 
                     sizeof(short int) + sizeof(short int) + 
                     sizeof(char[10]);
    
    int num_records = file_size / record_size;
    
    for (int i = 0; i < num_records; i++) {
        Citizen* new_citizen = (Citizen*)malloc(sizeof(Citizen));
        
        // Чтение каждого поля отдельно для избежания проблем с выравниванием
        if (fread(new_citizen->full_name, sizeof(char), 32, file) != 32) break;
        if (fread(new_citizen->street, sizeof(char), 18, file) != 18) break;
        if (fread(&new_citizen->house, sizeof(short int), 1, file) != 1) break;
        if (fread(&new_citizen->apartment, sizeof(short int), 1, file) != 1) break;
        if (fread(new_citizen->settlement_date, sizeof(char), 10, file) != 10) break;
        
        // Гарантируем нулевое завершение строк
        new_citizen->full_name[31] = '\0';
        new_citizen->street[17] = '\0';
        new_citizen->settlement_date[9] = '\0';
        
        new_citizen->next = NULL;
        
        if (head == NULL) {
            head = tail = new_citizen;
        } else {
            tail->next = new_citizen;
            tail = new_citizen;
        }
        
        (*count)++;
    }
    
    fclose(file);
    return head;
}

void freeList(Citizen* head) {
    Citizen* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main() {
    system("chcp 65001 > nul");
    int count;
    Citizen* head = loadFromFile("testBase4.dat", &count);
    
    if (head == NULL) {
        printf("Файл не найден. Создание тестовых данных...\n");
        
        count = 1;
        head = (Citizen*)malloc(sizeof(Citizen));
        
        // Используем strncpy для безопасности
        strncpy(head->full_name, "Санни_Кевиновна_Каслана", 31);
        head->full_name[31] = '\0';
        
        strncpy(head->street, "Гипостазис", 17);
        head->street[17] = '\0';
        
        head->house = 10;
        head->apartment = 15;
        
        strncpy(head->settlement_date, "15-03-85", 9);
        head->settlement_date[9] = '\0';
        
        head->next = NULL;
    }    

    Citizen** index = createIndexArray(head, count);
    int total_records = count;
    int page = 0;
    char command;

    do {
        system("cls");
        
        int start = page * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        if (end > total_records) end = total_records;
        
        printf("%-35s %-22s %-8s %-12s %-15s\n", 
               "Фио", "Улица", "Дом", "Квартира", "Дата");
        printf("-------------------------------------------------------------------------------\n");
        
        for (int i = start; i < end; ++i) {
            printf("%-32s %-18s %-6hd %-10hd %-10s\n", 
                   index[i]->full_name, 
                   index[i]->street, 
                   index[i]->house, 
                   index[i]->apartment, 
                   index[i]->settlement_date);
        }

        printf("\nПоказаны записи %d–%d из %d\n", start + 1, end, total_records);
        printf("[n] - следующая, [p] - предыдущая, [s] - сортировать, [f] - найти по году, [q] - выход: ");
        scanf(" %c", &command);
        while (getchar() != '\n'); // очистка буфера
        
        if (command == 'n' || command == 'N') {
            if ((page + 1) * PAGE_SIZE < total_records)
                page++;
        }
        else if (command == 'p' || command == 'P') {
            if (page > 0)
                page--;
        }
        else if (command == 's' || command == 'S') {
            // Сортировка
            mergeSort(&head);
            free(index);
            index = createIndexArray(head, count);
            printf("Данные отсортированы!\n");
            printf("Нажмите Enter для продолжения...");
            getchar();
        }
        else if (command == 'f' || command == 'F') {
            char search_year[3];
            printf("Введите год для поиска (2 цифры): ");
            scanf("%2s", search_year);
            
            int result_count;
            Citizen** results = searchByYear(index, count, search_year, &result_count);
            
            if (result_count > 0) {
                printf("\nНайдено %d записей за %s год:\n", result_count, search_year);
                printf("=======================================================================\n");
                for (int i = 0; i < result_count; i++) {
                    printCitizen(results[i]);
                }
                free(results);
            } else {
                printf("Записей за %s год не найдено.\n", search_year);
            }
            printf("Нажмите Enter для продолжения...");
            getchar();
        }

    } while (command != 'q' && command != 'Q');

    free(index);
    freeList(head);
    
    return 0;
}