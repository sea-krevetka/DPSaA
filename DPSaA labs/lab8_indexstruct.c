#include <stdio.h>
#include <string.h>

#define MAX_RECORDS 5
#define MAX_CONTACTS 5

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

int lessPhonethenName(Contact x, Contact y) {
    int phoneCompare = strcmp(x.phone, y.phone);
    if (phoneCompare < 0) return 1;
    else if (phoneCompare > 0) return 0;
    else return (strcmp(x.name, y.name) < 0);
}

int lessNamethenPhone(Contact x, Contact y) {
    int nameCompare = strcmp(x.name, y.name);
    if (nameCompare < 0) return 1;
    else if (nameCompare > 0) return 0;
    else return (strcmp(x.phone, y.phone) < 0);
}

int lesssurNamethenPhone(Contact x, Contact y) {
    int nameCompare = strcmp(x.surname, y.surname);
    if (nameCompare < 0) return 1;
    else if (nameCompare > 0) return 0;
    else return (strcmp(x.surname, y.surname) < 0);
}

void createIndexArray(Contact phoneBook[], int index[], int n, int (*less)(Contact, Contact)) {
    for (int i = 0; i < n; i++) {
        index[i] = i;
    }
    int start = 0;
    int end = n - 1;
    while (start < end) {
        for (int i = start; i < end; i++) {
            if (less(phoneBook[index[i + 1]], phoneBook[index[i]])) {
                int temp = index[i];
                index[i] = index[i + 1];
                index[i + 1] = temp;
            }
        }
        end--;
        for (int i = end; i > start; i--) {
            if (less(phoneBook[index[i]], phoneBook[index[i - 1]])) {
                int temp = index[i];
                index[i] = index[i - 1];
                index[i - 1] = temp;
            }
        }
        start++;
    }
}


void printPhoneBookByIndex(Contact phoneBook[], int index[], int n) {
    for (int i = 0; i < n; i++) {
        int idx = index[i];
        printf("%s %s, Телефон: %s, Email: %s", phoneBook[idx].name, phoneBook[idx].surname, phoneBook[idx].phone, phoneBook[idx].email);
        printf("\n");
    }
    printf("\n");
}

void printIndexArrays(int nameIndex[], int phoneIndex[], int n) {
    printf("Индексный массив для имени:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", nameIndex[i]);
    }
    printf("\n");

    printf("Индексный массив для телефона:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", phoneIndex[i]);
    }
    printf("\n");
}

int main() {
    Contact phoneBook[MAX_RECORDS] = {
        {"Ivan", "Petrov", "123-456", "ivan@mail.com"},
        {"Pavel", "Sidorov", "789-012", "pavel@mail.com"},
        {"Anna", "Ivanova", "345-678", "anna@mail.com"},
        {"Tatiana", "Petrova", "234-567", "tatiana@mail.com"},
        {"Sergey", "Sidorov", "456-789", "sergey@mail.com"}
    };

    int nameIndex[MAX_RECORDS];
    int phoneIndex[MAX_RECORDS];

    printf("Исходный телефонный справочник:\n");
    printContacts(phoneBook, MAX_CONTACTS);

    createIndexArray(phoneBook, nameIndex, MAX_RECORDS, lessNamethenPhone);
    createIndexArray(phoneBook, phoneIndex, MAX_RECORDS, lessPhonethenName);

    printf("\nСортировка по имени (с использованием индексного массива):\n");
    printPhoneBookByIndex(phoneBook, nameIndex, MAX_RECORDS);

    printf("Сортировка по телефону (с использованием индексного массива):\n");
    printPhoneBookByIndex(phoneBook, phoneIndex, MAX_RECORDS);
    
    printIndexArrays(nameIndex, phoneIndex, MAX_RECORDS);

    return 0;
}
