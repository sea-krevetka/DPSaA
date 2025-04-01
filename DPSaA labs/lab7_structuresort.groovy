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

int compareContactsByPhonenum(const Contact *a, const Contact *b, int UporDown) {
    if (UporDown == 1) {
        return strcmp(a->phone, b->phone);
    } else {
        return strcmp(b->phone, a->phone);
    }
}

int compareContactsByemail(const Contact *a, const Contact *b, int UporDown) {
    if (UporDown == 1) {
        return strcmp(a->email, b->email);
    } else {
        return strcmp(b->email, a->email);
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

int binarySearch(Contact contacts[], int size, const char *keySurname) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int comparison = strcmp(contacts[mid].surname, keySurname);

        if (comparison == 0) {
            return mid;
        }
        if (comparison < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1; 
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
    
    insertionSortContacts(contacts, MAX_CONTACTS, compareContactsByPhonenum,1);
    printf("\nОтсортированный справочник по номеру:\n");
    printContacts(contacts, MAX_CONTACTS);

    insertionSortContacts(contacts, MAX_CONTACTS, compareContactsByemail,1);
    printf("\nОтсортированный справочник по почте:\n");
    printContacts(contacts, MAX_CONTACTS);
    
    
    
    
    

    printf("\n\n\n\n\nПоиск");
    insertionSortContacts(contacts, MAX_CONTACTS, compareContactsBySurnameAndName, 1);
    char *searchKey = "Petrov";
    
    int result = binarySearch(contacts, MAX_CONTACTS, searchKey);

    if (result != -1) {
        printf("\nПоиск по фамилии '%s': Найдено - %s %s, Телефон: %s, Email: %s\n", 
            searchKey, contacts[result].name, contacts[result].surname, contacts[result].phone, contacts[result].email);
    } else {
        printf("\nПоиск по фамилии '%s': Не найдено\n", searchKey);
    }

    return 0;
}
