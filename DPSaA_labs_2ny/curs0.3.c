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

int less(const record *a, const record *b) {
    int day_a   = (a->date[0] - '0') * 10 + (a->date[1] - '0');
    int month_a = (a->date[3] - '0') * 10 + (a->date[4] - '0');
    int year_a  = (a->date[6] - '0') * 10 + (a->date[7] - '0');

    int day_b   = (b->date[0] - '0') * 10 + (b->date[1] - '0');
    int month_b = (b->date[3] - '0') * 10 + (b->date[4] - '0');
    int year_b  = (b->date[6] - '0') * 10 + (b->date[7] - '0');

    if (year_a != year_b)
        return year_a < year_b;

    if (month_a != month_b)
        return month_a < month_b;

    if (day_a != day_b)
        return day_a < day_b;

    char street_a[18], street_b[18];
    strncpy(street_a, a->street, sizeof(street_a) - 1);
    street_a[sizeof(street_a) - 1] = '\0';
    strncpy(street_b, b->street, sizeof(street_b) - 1);
    street_b[sizeof(street_b) - 1] = '\0';

    for (int i = 0; street_a[i]; i++)
        street_a[i] = tolower((unsigned char)street_a[i]);
    for (int i = 0; street_b[i]; i++)
        street_b[i] = tolower((unsigned char)street_b[i]);

    return strcmp(street_a, street_b) < 0;
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

void binarySearchByYearRange(record *records, int records_count, int year_from, int year_to, int **results, int *results_count);

void displayPage(record records[], int indices[], int startIdx, int endIdx, int current_page, int total_pages, int total_records, const char* title, int use_indices) {
    system("cls");
    system("chcp 65001 > nul");
    printf("%s\n", title);
    printf("Страница: %d/%d. Записи с %d по %d из %d.\n", 
           current_page + 1, total_pages, startIdx + 1, endIdx, total_records);
    printf("------------------------------------------------------------------------------------------\n");
    printf("|%-4s | %-35s | %-23s | %-9s | %-6s | %-14s|\n",
           "№", "ФИО", "Улица", "Дом", "Квартира", "Дата");
    printf("------------------------------------------------------------------------------------------\n");
    system("chcp 866 > nul");
    
    for (int i = startIdx; i < endIdx; i++) {
        int record_index = use_indices ? indices[i] : i;
        printf("|%-4d | %-32s | %-18s | %-6hd | %-8hd | %-10s|\n",
               i + 1,
               records[record_index].FIO,
               records[record_index].street,
               records[record_index].house,
               records[record_index].apartment_num,
               records[record_index].date);
    }
    
    printf("------------------------------------------------------------------------------------------\n");
    system("chcp 65001 > nul");
}

void displaySortedArray(record records[], int records_count, const char* title) {
    system("cls");
    system("chcp 65001 > nul");
    printf("%s\n", title);
    printf("Всего записей: %d\n", records_count);
    printf("------------------------------------------------------------------------------------------\n");
    printf("|%-4s | %-35s | %-23s | %-9s | %-6s | %-14s|\n",
           "№", "ФИО", "Улица", "Дом", "Квартира", "Дата");
    printf("------------------------------------------------------------------------------------------\n");
    system("chcp 866 > nul");
    
    for (int i = 0; i < records_count; i++) {
        printf("|%-4d | %-32s | %-18s | %-6hd | %-8hd | %-10s|\n",
               i + 1,
               records[i].FIO,
               records[i].street,
               records[i].house,
               records[i].apartment_num,
               records[i].date);
    }
    
    printf("------------------------------------------------------------------------------------------\n");
    system("chcp 65001 > nul");
    printf("\nНажмите любую клавишу для возврата...");
    getchar();
    getchar();
}

void binarySearchByYearRange(record *records, int records_count, int year_from, int year_to, int **results, int *results_count) {
    int left = 0, right = records_count - 1;
    int startIdx = -1, endIdx = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int year_mid = (records[mid].date[6] - '0') * 10 + (records[mid].date[7] - '0');
        if (year_mid < year_from)
            left = mid + 1;
        else {
            startIdx = mid;
            right = mid - 1;
        }
    }

    left = 0;
    right = records_count - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        int year_mid = (records[mid].date[6] - '0') * 10 + (records[mid].date[7] - '0');
        if (year_mid > year_to)
            right = mid - 1;
        else {
            endIdx = mid;
            left = mid + 1;
        }
    }

    *results_count = 0;
    if (startIdx != -1 && endIdx != -1 && startIdx <= endIdx) {
        *results_count = endIdx - startIdx + 1;
        *results = malloc(*results_count * sizeof(int));
        for (int i = 0; i < *results_count; i++) {
            (*results)[i] = startIdx + i;
        }
    } else {
        *results = NULL;
    }
}

void navigatePages(record *records, int records_count, const char* title){
    int current_page = 0;
    char input;
    char year[3];
    int *search_results = NULL;
    int search_count = 0;
    int is_search_mode = 0;

    do{
        int total_pages;
        int startIdx, endIdx;
        int current_count;

        if (is_search_mode && search_results != NULL) {
            current_count = search_count;
            total_pages = (search_count + RECORDS_ON_PAGE - 1) / RECORDS_ON_PAGE;
            startIdx = current_page * RECORDS_ON_PAGE;
            endIdx = startIdx + RECORDS_ON_PAGE;
            if (endIdx > search_count) endIdx = search_count;
        } else {
            current_count = records_count;
            total_pages = (records_count + RECORDS_ON_PAGE - 1) / RECORDS_ON_PAGE;
            startIdx = current_page * RECORDS_ON_PAGE;
            endIdx = startIdx + RECORDS_ON_PAGE;
            if (endIdx > records_count) endIdx = records_count;
        }

        if (total_pages == 0) total_pages = 1;

        if (current_page >= total_pages){
            current_page = total_pages - 1;
        }
        if (current_page < 0){
            current_page = 0;
        }

        char page_title[100];
        if (is_search_mode && search_results != NULL) {
            snprintf(page_title, sizeof(page_title), "%s (Поиск: %d записей)", title, search_count);
        } else {
            strcpy(page_title, title);
        }

        if (is_search_mode && search_results != NULL) {
            displayPage(records, search_results, startIdx, endIdx, current_page, total_pages, 
                       search_count, page_title, 1);
        } else {
            displayPage(records, NULL, startIdx, endIdx, current_page, total_pages, 
                       records_count, page_title, 0);
        }

        printf("\nУправление: [A] - предыдущая, [D] - следующая, [G] - перейти к странице\n");
        printf("           [S] - первая страница, [E] - последняя страница, [J] - назад на 10, [L] - вперед на 10\n");
        printf("           [F] - поиск по году, [C] - очистить поиск, [V] - вывести весь массив, [Q] - выйти\n");
        if (is_search_mode) {
            printf("           *** РЕЖИМ ПОИСКА АКТИВЕН ***\n");
        }
        printf("Введите команду: ");

        scanf(" %c", &input);

        switch(input){
            case 'A': case 'a':
                if (current_page > 0) current_page--;
                break;
                
            case 'D': case 'd':
                if (current_page < total_pages - 1) current_page++;
                break;
                
            case 'S': case 's':
                current_page = 0;
                break;
                
            case 'E': case 'e':
                current_page = total_pages - 1;
                break;
                
            case 'J': case 'j':
                if (current_page - 10 >= 0) {
                    current_page -= 10;
                } else {
                    current_page = 0;
                }
                break;
                
            case 'L': case 'l':
                if (current_page + 10 < total_pages) {
                    current_page += 10;
                } else {
                    current_page = total_pages - 1;
                }
                break;
                
            case 'G': case 'g':
                {
                    int target_page;
                    printf("Введите номер страницы (1-%d): ", total_pages);
                    if (scanf("%d", &target_page) == 1) {
                        if (target_page >= 1 && target_page <= total_pages) {
                            current_page = target_page - 1;
                        } else {
                            printf("Неверный номер страницы! Нажмите любую клавишу...");
                            getchar(); getchar();
                        }
                    }
                }
                break;
                
            case 'F': case 'f':
                {
                    printf("Введите год для поиска (93-99): ");
                    if (scanf("%2s", year) == 1) {
                        int search_year = (year[0] - '0') * 10 + (year[1] - '0');
                        
                        if (search_results != NULL) {
                            free(search_results);
                            search_results = NULL;
                        }
                        
                        binarySearchByYearRange(records, records_count, search_year, search_year, &search_results, &search_count);
                        
                        if (search_count > 0) {
                            is_search_mode = 1;
                            current_page = 0;
                            printf("Найдено записей: %d. Нажмите любую клавишу...", search_count);
                        } else {
                            printf("Записей за год %s не найдено. Нажмите любую клавишу...", year);
                        }
                        getchar(); getchar();
                    }
                }
                break;
                
            case 'C': case 'c':
                if (search_results != NULL) {
                    free(search_results);
                    search_results = NULL;
                }
                is_search_mode = 0;
                search_count = 0;
                current_page = 0;
                break;
                
            case 'V': case 'v':
                {
                    char full_title[100];
                    if (is_search_mode && search_results != NULL) {
                        snprintf(full_title, sizeof(full_title), "%s - Полный вывод (Поиск: %d записей)", title, search_count);
                        record *temp_records = malloc(search_count * sizeof(record));
                        for (int i = 0; i < search_count; i++) {
                            temp_records[i] = records[search_results[i]];
                        }
                        displaySortedArray(temp_records, search_count, full_title);
                        free(temp_records);
                    } else {
                        snprintf(full_title, sizeof(full_title), "%s - Полный вывод", title);
                        displaySortedArray(records, records_count, full_title);
                    }
                }
                break;
                
            case 'Q': case 'q':
                if (search_results != NULL) {
                    free(search_results);
                }
                return;
                
            default:
                printf("Неизвестная команда! Нажмите любую клавишу...");
                getchar(); getchar();
                break;
        }

    } while (1);
}

void fillQ(tData **head, tData **tail, record *Records, int records_count){
    for (int i = 0; i < records_count; i++){
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
    int records_count = fread(allRecords, sizeof(record), N, fp);
    
    if (records_count == 0) {
        printf("Не удалось прочитать записи из файла!\n");
        fclose(fp);
        free(allRecords);
        return 1;
    }

    tData *head = NULL;
    tData *tail = NULL;
    fillQ(&head, &tail, allRecords, records_count);
    MergeSort(&head);
    
    tData *current = head;
    record *sortedRecords = malloc(records_count * sizeof(record));
    for (int i = 0; i < records_count && current != NULL; i++){
        sortedRecords[i] = *(current->data);
        current = current->next;
    }
    clear(head);

    navigatePages(sortedRecords, records_count, "База данных - Отсортированные данные");

    fclose(fp);
    free(allRecords);
    free(sortedRecords);
    return 0;
}