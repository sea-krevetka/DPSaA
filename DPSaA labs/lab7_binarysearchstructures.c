#include <stdio.h>
#include <string.h>

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

int compareContactsBySurnameAndName(const Contact *a, const Contact *b, int UporDown) {
    int surnameCompare = strcmp(a->surname, b->surname);
    if (UporDown == 1) {
        if (surnameCompare == 0) {
            return strcmp(a->name, b->name);
        }
        return surnameCompare;
    } else {
        if (surnameCompare == 0) {
            return strcmp(b->name, a->name);
        }
        return -surnameCompare;
    }
}

void insertionSortContacts(Contact contacts[], int size, int (*compare)(const Contact*, const Contact*, int), int UporDown) {
    for (int i = 1; i < size; i++) {
        Contact key = contacts[i];
        int j = i - 1;

        while (j >= 0 && compare(&contacts[j], &key, UporDown) > 0) {
            contacts[j + 1] = contacts[j];
            j--;
        }
        contacts[j + 1] = key;
    }
}

void searchBySurname(Contact contacts[], int size, const char *keySurname) {
    printf("\nПоиск по фамилии '%s':\n", keySurname);
    int found = 0; // Флаг для проверки, найдены ли контакты
    for (int i = 0; i < size; i++) {
        if (strcmp(contacts[i].surname, keySurname) == 0) {
            printf("Найдено - %s %s, Телефон: %s, Email: %s\n", 
                   contacts[i].name, contacts[i].surname, contacts[i].phone, contacts[i].email);
            found = 1;
        }
    }
    if (!found) {
        printf("Не найдено\n");
    }
}

int main() {
    Contact contacts[MAX_CONTACTS] = {
        {"Ivan", "Petrov", "123-456", "ivan@mail.com"},
        {"Pavel", "Sidorov", "789-012", "pavel@mail.com"},
        {"Anna", "Ivanova", "345-678", "anna@mail.com"},
        {"Tatiana", "Petrova", "234-567", "tatiana@mail.com"},
        {"Sergey", "Sidorov", "456-789", "sergey@mail.com"}
    };

    printf("Исходный справочник:\n");
    printContacts(contacts, MAX_CONTACTS);

    insertionSortContacts(contacts, MAX_CONTACTS, compareContactsBySurnameAndName, 1);
    printf("\nОтсортированный справочник по фамилии и имени:\n");
    printContacts(contacts, MAX_CONTACTS);
    
    // Поиск всех контактов с одинаковой фамилией
    char searchKey[50]; // Изменено на массив, чтобы избежать ошибки сегментации
    printf("Введите фамилию для поиска: \n");
    scanf("%49s", searchKey);
    searchBySurname(contacts, MAX_CONTACTS, searchKey);

    return 0;
}
