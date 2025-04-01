#include <stdio.h>
#include <string.h>

#define MAX_RECORDS 6

typedef struct {
    char name[50];
    char surname[50];
    char phone[15];
    char email[50];
} Contact;

void printContacts(Contact contacts[], int size) {
    printf("Телефонный справочник:\n");
    for (int i = 0; i < size; i++) {
        printf("%s %s, Телефон: %s, Email: %s\n", contacts[i].name, contacts[i].surname, contacts[i].phone, contacts[i].email);
    }
}

int lessThan(Contact a, Contact b) {
    return strcmp(a.name, b.name) < 0;
}

void createIndexArray(Contact phoneBook[], int index[], int n, int (*less)(Contact, Contact)) {
    for (int i = 0; i < n; i++) {
        index[i] = i;
    }
    
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (less(phoneBook[index[j]], phoneBook[index[minIndex]])) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = index[i];
            index[i] = index[minIndex];
            index[minIndex] = temp;
        }
    }
}

void printPhoneBookByIndex(Contact phoneBook[], int index[], int size) {
    for (int i = 0; i < size; i++) {
        int idx = index[i];
        printf("%s %s, Телефон: %s, Email: %s\n", phoneBook[idx].name, phoneBook[idx].surname, phoneBook[idx].phone, phoneBook[idx].email);
    }
}

void binarySearchByName(Contact phoneBook[], int nameIndex[], int n, char *key) {
    int left = 0, right = n - 1;
    int first = -1;
    printf("Результаты поиска по имени %s:\n", key);

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(phoneBook[nameIndex[mid]].name, key);
        
        if (cmp == 0) {
            first = mid;
            right = mid - 1;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (first != -1) {
        for (int i = first; i < n && strcmp(phoneBook[nameIndex[i]].name, key) == 0; i++) {
            printf("%s %s, Телефон: %s, Email: %s\n", 
                   phoneBook[nameIndex[i]].name, 
                   phoneBook[nameIndex[i]].surname, 
                   phoneBook[nameIndex[i]].phone, 
                   phoneBook[nameIndex[i]].email);
        }
    } else {
        printf("Нет результатов.\n");
    }
}
void binarySearchBySurname(Contact phoneBook[], int surnameIndex[], int n, char *key) {
    int left = 0, right = n - 1;
    int first = -1;
    printf("Результаты поиска по фамилии %s:\n", key);
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(phoneBook[surnameIndex[mid]].surname, key);
        
        if (cmp == 0) {
            first = mid;
            right = mid - 1;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (first != -1) {
        for (int i = first; i < n && strcmp(phoneBook[surnameIndex[i]].surname, key) == 0; i++) {
            printf("%s %s, Телефон: %s, Email: %s\n", 
                   phoneBook[surnameIndex[i]].name, 
                   phoneBook[surnameIndex[i]].surname, 
                   phoneBook[surnameIndex[i]].phone, 
                   phoneBook[surnameIndex[i]].email);
        }
    } else {
        printf("Нет результатов.\n");
    }
}

int main() {
    Contact phoneBook[MAX_RECORDS] = {
        {"Ivan", "Petrov", "123-456", "ivan@mail.com"},
        {"Pavel", "Sidorov", "789-012", "pavel@mail.com"},
        {"Anna", "Ivanova", "345-678", "anna@mail.com"},
        {"Tatiana", "Petrova", "234-567", "tatiana@mail.com"},
        {"Sergey", "Sidorov", "456-789", "sergey@mail.com"},
        {"Anna", "Kalinina", "336-138", "Kalin@mail.com"}
    };

    int nameIndex[MAX_RECORDS];
    int surnameIndex[MAX_RECORDS];

    printf("Исходный телефонный справочник:\n");
    printContacts(phoneBook, MAX_RECORDS);

    createIndexArray(phoneBook, nameIndex, MAX_RECORDS, lessThan);
    createIndexArray(phoneBook, surnameIndex, MAX_RECORDS, lessThan);

    printf("\nВведите имя для поиска: ");
    char name[50];
    scanf("%s", name);
    binarySearchByName(phoneBook, nameIndex, MAX_RECORDS, name);

    printf("\nВведите фамилию для поиска: ");
    char surname[50];
    scanf("%s", surname);
    binarySearchBySurname(phoneBook, surnameIndex, MAX_RECORDS, surname);

    return 0;
}
