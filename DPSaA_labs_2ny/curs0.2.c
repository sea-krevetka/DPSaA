#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 4000
#define RECORDS_ON_PAGE 20

typedef struct record{
    char FIO[32];
    char street[18];
    short int house;
    short int apartment_num;
    char date[10];
} record;

typedef struct tData{
    struct tData *next;
    record *data;
} tData;

void AddToQueue(tData **head, tData **tail, record *data){
    tData *p = (tData *)malloc(sizeof(tData));
    p->data = data;
    p->next = NULL;

    if (*tail == NULL){
        *head = *tail = p;
    }
    else{
        (*tail)->next = p;
        *tail = p;
    }
}

void clear(tData *head){
    tData *p = head;
    while (p != NULL){
        tData *temp_p = p;
        p = p->next;
        free(temp_p);
    }
}

typedef struct{
    tData *head;
    tData *tail;
} tQueue;

int less(const record *a, const record *b){

    int year_a = (a->date[6] - '0') * 10 + (a->date[7] - '0');
    int year_b = (b->date[6] - '0') * 10 + (b->date[7] - '0');
    
    if (year_a != year_b) {
        return year_a < year_b;
    }
    
    int month_a = (a->date[3] - '0') * 10 + (a->date[4] - '0');
    int month_b = (b->date[3] - '0') * 10 + (b->date[4] - '0');
    
    if (month_a != month_b) {
        return month_a < month_b;
    }
    
    int day_a = (a->date[0] - '0') * 10 + (a->date[1] - '0');
    int day_b = (b->date[0] - '0') * 10 + (b->date[1] - '0');
    
    if (day_a != day_b) {
        return day_a < day_b;
    }
    
    char street_a[18], street_b[18];
    strncpy(street_a, a->street, sizeof(street_a) - 1);
    street_a[sizeof(street_a) - 1] = '\0';
    strncpy(street_b, b->street, sizeof(street_b) - 1);
    street_b[sizeof(street_b) - 1] = '\0';
    
    for (int i = 0; street_a[i]; i++)
        street_a[i] = tolower((unsigned char)street_a[i]);
    for (int i = 0; street_b[i]; i++)
        street_b[i] = tolower((unsigned char)street_b[i]);
    
    int street_cmp = strcmp(street_a, street_b);
    if (street_cmp < 0) {
        return 1;
    } else if (street_cmp > 0) {
        return 0;
    }
    
    return 1;
}
void InitializeQueue(tQueue *q){
    q->head = q->tail = NULL;
}

void MergeSeries(tData **a, int q, tData **b, int r, tQueue *c){
    while (q > 0 && r > 0 && *a != NULL && *b != NULL){
        if (less((*a)->data, (*b)->data)){
            if (c->head == NULL){
                c->head = c->tail = *a;
            }
            else{
                c->tail->next = *a;
                c->tail = *a;
            }
            *a = (*a)->next;
            q--;
        }
        else{
            if (c->head == NULL){
                c->head = c->tail = *b;
            }
            else{
                c->tail->next = *b;
                c->tail = *b;
            }
            *b = (*b)->next;
            r--;
        }
    }

    while (q > 0 && *a != NULL){
        if (c->head == NULL){
            c->head = c->tail = *a;
        }
        else{
            c->tail->next = *a;
            c->tail = *a;
        }
        *a = (*a)->next;
        q--;
    }

    while (r > 0 && *b != NULL){
        if (c->head == NULL){
            c->head = c->tail = *b;
        }
        else{
            c->tail->next = *b;
            c->tail = *b;
        }
        *b = (*b)->next;
        r--;
    }

    if (c->tail != NULL){
        c->tail->next = NULL;
    }
}

void SplitLists(tData *head, tData **a, tData **b, int *n){
    *a = head;
    *b = head->next;

    tData *k = *a, *p = *b;
    *n = 1;

    while (p != NULL){
        (*n)++;
        k->next = p->next;
        k = p;
        p = p->next;
    }
}

void MergeSort(tData **S)
{
    if (*S == NULL || (*S)->next == NULL){
        return;
    }

    tQueue c[2];
    tData *a, *b;
    int n;
    SplitLists(*S, &a, &b, &n);

    for (int p = 1; p < n; p *= 2){
        InitializeQueue(&c[0]);
        InitializeQueue(&c[1]);
        int i = 0, m = n;

        while (m > 0){
            int q = (m >= p) ? p : m;
            m -= q;

            int r = (m >= p) ? p : m;
            m -= r;

            MergeSeries(&a, q, &b, r, &c[i]);

            i = 1 - i;
        }

        a = c[0].head;
        b = c[1].head;
    }

    if (c[0].tail != NULL){
        c[0].tail->next = NULL;
    }
    *S = c[0].head;
}

void navigatePages(record *allRecords, int records_count){
    int current_page = 0;
    char input;

    do{
        system("cls");

        int pages_count = records_count / RECORDS_ON_PAGE;
        if (records_count % RECORDS_ON_PAGE != 0){
            pages_count++;
        }

        if (current_page >= pages_count){
            current_page = pages_count - 1;
        }
        if (current_page < 0){
            current_page = 0;
        }

        int pageStart = current_page * RECORDS_ON_PAGE;
        int pageEnd = pageStart + RECORDS_ON_PAGE;
        if (pageEnd > records_count){
            pageEnd = records_count;
        }
        system("chcp 65001 > nul");
        printf("Страница: %d/%d. Записи с %d по %d из %d.\n",
               current_page + 1, pages_count, pageStart + 1, pageEnd, records_count);
        printf("------------------------------------------------------------------------------------------\n");
        printf("|%-4s | %-35s | %-23s | %-9hs | %-6hs | %-14s|\n",
               "№", "ФИО", "Улица", "Дом", "Квартира", "Дата");
        printf("------------------------------------------------------------------------------------------\n");
        system("chcp 866 > nul");
        for (int i = pageStart; i < pageEnd; i++)
        {
            printf("|%-4d | %-32s | %-18s | %-6hd | %-8hd | %-10s|\n",
                   i + 1,
                   allRecords[i].FIO,
                   allRecords[i].street,
                   allRecords[i].house,
                   allRecords[i].apartment_num,
                   allRecords[i].date);
            
        }
        printf("------------------------------------------------------------------------------------------\n");
        system("chcp 65001 > nul");
        printf("\nУправление: [A] - предыдущая, [D] - следующая, [S] - вернуться в начало, [E] - перейти в конец [J] - назад на 10 страниц, [L] - вперед на 10 страниц, [W] - Сортировать\n");
        printf("Введите команду: ");

        scanf(" %c", &input);

        if (((input == 'A') || (input=='a')) && current_page > 0){
            current_page--;
        }
        else if (((input == 'D') || (input=='d'))){
            if (current_page < pages_count - 1)
            {
                current_page++;
            }
        }
        else if (((input == 'S') || (input=='s'))){
            current_page = 0;
        }
        else if (((input == 'E') || (input=='e'))){
            current_page = pages_count - 1;
        }
        else if (((input == 'J') || (input=='j'))){
            current_page -= 10;
            if (current_page < 0) current_page = 0;
        }
        else if (((input == 'L') || (input=='l'))){
            current_page += 10;
            if (current_page >= pages_count) current_page = pages_count - 1;
        }
        else if (((input == 'W') || (input=='w'))){
            break;
        }

    } while (1);
}

void fillQ(tData **head, tData **tail, record *Records){
    for (int i = 0; i < N; i++){
        AddToQueue(head, tail, &Records[i]);
    }
}

int main(){
    FILE *fp;
    fp = fopen("testBase4.dat", "rb");
    if (fp == NULL){
        printf("Ошибка открытия файла!\n");
        return 1;
    }

    record *allRecords = malloc(N * sizeof(record));
    int records_count = fread(allRecords, sizeof(record), 4000, fp);
    int current_page = 0;
    navigatePages(allRecords, N);

    tData *head = NULL;
    tData *tail = NULL;

    fillQ(&head, &tail, allRecords);
    MergeSort(&head);
    tData *current = head;
    record *sortedRecords = malloc(records_count * sizeof(record));

    for (int i = 0; i < records_count && current != NULL; i++){
        sortedRecords[i] = *(current->data);
        current = current->next;
    }

    clear(head);
    navigatePages(sortedRecords, N);

    fclose(fp);
    free(allRecords);
    free(sortedRecords);
    return 0;
}