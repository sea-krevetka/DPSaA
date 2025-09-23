#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct Vertex {
    int data;
    struct Vertex* left;
    struct Vertex* right;
} Vertex;

Vertex* add_recursive(Vertex* p, int data) {
    if (p == NULL) {
        p = (Vertex*)malloc(sizeof(Vertex));
        p->data = data;
        p->left = NULL;
        p->right = NULL;
    } else if (data < p->data) {
        p->left = add_recursive(p->left, data);
    } else if (data > p->data) {
        p->right = add_recursive(p->right, data);
    }
    return p;
}

void add_double_indirect(Vertex** p, int data) {
    while (*p != NULL) {
        if (data < (*p)->data) {
            p = &((*p)->left);
        } else if (data > (*p)->data) {
            p = &((*p)->right);
        } else {
            return;
        }
    }
    
    *p = (Vertex*)malloc(sizeof(Vertex));
    (*p)->data = data;
    (*p)->left = NULL;
    (*p)->right = NULL;
}

Vertex* build_ISDP(int arr[], int L, int R) {
    if (L > R) return NULL;
    
    int m = (L + R) / 2;
    Vertex* p = (Vertex*)malloc(sizeof(Vertex));
    p->data = arr[m];
    p->left = build_ISDP(arr, L, m - 1);
    p->right = build_ISDP(arr, m + 1, R);
    return p;
}

Vertex* find_min(Vertex* p) {
    if (p == NULL) return NULL;
    while (p->left != NULL) {
        p = p->left;
    }
    return p;
}

Vertex* delete_vertex(Vertex* root, int key) {
    if (root == NULL) return NULL;
    
    if (key < root->data) {
        root->left = delete_vertex(root->left, key);
    } else if (key > root->data) {
        root->right = delete_vertex(root->right, key);
    } else {
        // Найден узел для удаления
        if (root->left == NULL) {
            Vertex* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Vertex* temp = root->left;
            free(root);
            return temp;
        } else {
            // У узла есть два потомка
            Vertex* temp = find_min(root->right);
            root->data = temp->data;
            root->right = delete_vertex(root->right, temp->data);
        }
    }
    return root;
}

void inorder_traversal(Vertex* root) {
    if (root != NULL) {
        inorder_traversal(root->left);
        printf("%d ", root->data);
        inorder_traversal(root->right);
    }
}

void generate_dot(Vertex* root, FILE* file) {
    if (root == NULL) return;
    
    if (root->left != NULL) {
        fprintf(file, "    %d -> %d;\n", root->data, root->left->data);
        generate_dot(root->left, file);
    }
    
    if (root->right != NULL) {
        fprintf(file, "    %d -> %d;\n", root->data, root->right->data);
        generate_dot(root->right, file);
    }
}

void add_nodes(Vertex* r, FILE* file) {
    if (r == NULL) return;
    fprintf(file, "    %d;\n", r->data);
    add_nodes(r->left, file);
    add_nodes(r->right, file);
}

void save_tree_to_dot(Vertex* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }
    
    fprintf(file, "digraph BST {\n");
    fprintf(file, "    node [shape=circle];\n");
    
    add_nodes(root, file);
    generate_dot(root, file);
    
    fprintf(file, "}\n");
    fclose(file);
}

void display_tree(Vertex* root, const char* title) {
    char filename[100];
    static int counter = 0;
    sprintf(filename, "tree_%d.dot", counter++);
    
    save_tree_to_dot(root, filename);
    printf("%s сохранен в %s\n", title, filename);
    
    char command[200];
    sprintf(command, "dot -Tpng %s -o tree_%d.png", filename, counter-1);
    system(command);
    printf("Изображение сохранено в tree_%d.png\n\n", counter-1);
}

int tree_size(Vertex* root) {
    if (root == NULL) return 0;
    return 1 + tree_size(root->left) + tree_size(root->right);
}

int tree_sum(Vertex* root) {
    if (root == NULL) return 0;
    return root->data + tree_sum(root->left) + tree_sum(root->right);
}

int tree_height(Vertex* root) {
    if (root == NULL) return 0;
    int left_height = tree_height(root->left);
    int right_height = tree_height(root->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

void calculate_average_height(Vertex* root, int level, int* total_path, int* total_nodes) {
    if (root == NULL) return;
    
    (*total_path) += level;
    (*total_nodes) += 1;
    
    calculate_average_height(root->left, level + 1, total_path, total_nodes);
    calculate_average_height(root->right, level + 1, total_path, total_nodes);
}

void free_tree(Vertex* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void quicksort(int arr[], int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2];
    int temp;
    
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
            j--;
        }
    }
    
    if (left < j) quicksort(arr, left, j);
    if (i < right) quicksort(arr, i, right);
}

void generate_unique_random_sequence(int arr[], int n, int max_value) {
    int* used = (int*)calloc(max_value, sizeof(int));
    int count = 0;
    
    while (count < n) {
        int num = rand() % max_value;
        if (!used[num]) {
            used[num] = 1;
            arr[count] = num;
            count++;
        }
    }
    
    free(used);
}

typedef struct KeywordNode {
    char* word;
    int frequency;
    struct KeywordNode* left;
    struct KeywordNode* right;
} KeywordNode;

// Список ключевых слов C
const char* c_keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    "NULL", "FILE", "true", "false", "bool", "include", "define", "sizeof"
};
const int num_keywords = 40;

KeywordNode* add_keyword(KeywordNode* root, const char* word) {
    if (root == NULL) {
        root = (KeywordNode*)malloc(sizeof(KeywordNode));
        root->word = strdup(word);
        root->frequency = 1;
        root->left = NULL;
        root->right = NULL;
    } else {
        int cmp = strcmp(word, root->word);
        if (cmp < 0) {
            root->left = add_keyword(root->left, word);
        } else if (cmp > 0) {
            root->right = add_keyword(root->right, word);
        } else {
            root->frequency++;
        }
    }
    return root;
}

int is_keyword(const char* word) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(word, c_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void extract_words(const char* text, KeywordNode** dictionary) {
    char buffer[100];
    int j = 0;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (isalnum(text[i]) || text[i] == '_') {
            buffer[j++] = tolower(text[i]);
        } else {
            if (j > 0) {
                buffer[j] = '\0';
                if (is_keyword(buffer)) {
                    *dictionary = add_keyword(*dictionary, buffer);
                }
                j = 0;
            }
        }
    }
}

void print_dictionary(KeywordNode* root) {
    if (root != NULL) {
        print_dictionary(root->left);
        printf("%-15s: %d\n", root->word, root->frequency);
        print_dictionary(root->right);
    }
}

void free_dictionary(KeywordNode* root) {
    if (root != NULL) {
        free_dictionary(root->left);
        free_dictionary(root->right);
        free(root->word);
        free(root);
    }
}
void walk(Vertex *p){
    if (p != NULL){
        printf("%d ", p->data);
        walk(p->left);
        walk(p->right);
    }
}

int main() {
    system("chcp 65001 > nul");
    srand(time(NULL));
    const int n = 10;
    const int max_value = 10000;
    int sequence[n];
    
    printf("=== ГЕНЕРАЦИЯ И АНАЛИЗ ДЕРЕВЬЕВ ===\n\n");
    
    printf("Сгенерированная последовательность:\n");
    generate_unique_random_sequence(sequence, n, max_value);
    for (int i = 0; i < n; i++) {
        printf("%d ", sequence[i]);
    }
    printf("\n\n");
    
    Vertex* root_recursive = NULL;
    for (int i = 0; i < n; i++) {
        root_recursive = add_recursive(root_recursive, sequence[i]);
    }
    
    Vertex* root_double = NULL;
    for (int i = 0; i < n; i++) {
        add_double_indirect(&root_double, sequence[i]);
    }
    
    int sorted_sequence[n];
    for (int i = 0; i < n; i++) {
        sorted_sequence[i] = sequence[i];
    }
    quicksort(sorted_sequence, 0, n - 1);
    Vertex* root_ISDP = build_ISDP(sorted_sequence, 0, n - 1);

    printf("Обход дерева:");
    walk(root_recursive);
    
    printf("Размеры деревьев:\n");
    printf("ИСДП: %d\n", tree_size(root_ISDP));
    printf("СДП1: %d\n", tree_size(root_recursive));
    printf("СДП2: %d\n\n", tree_size(root_double));
    
    int total_path, total_nodes;
    float avg_height;
    
    total_path = 0; total_nodes = 0;
    calculate_average_height(root_ISDP, 1, &total_path, &total_nodes);
    avg_height = (float)total_path / total_nodes;
    
    printf("n=%d\tРазмер\tКонтр.сумма\tВысота\tСредн.высота\n", n);
    printf("ИСДП\t%d\t%d\t\t%d\t%.2f\n", 
           tree_size(root_ISDP), tree_sum(root_ISDP), 
           tree_height(root_ISDP), avg_height);
    
    total_path = 0; total_nodes = 0;
    calculate_average_height(root_recursive, 1, &total_path, &total_nodes);
    avg_height = (float)total_path / total_nodes;
    
    printf("СДП1\t%d\t%d\t\t%d\t%.2f\n", 
           tree_size(root_recursive), tree_sum(root_recursive), 
           tree_height(root_recursive), avg_height);
    
    total_path = 0; total_nodes = 0;
    calculate_average_height(root_double, 1, &total_path, &total_nodes);
    avg_height = (float)total_path / total_nodes;
    
    printf("СДП2\t%d\t%d\t\t%d\t%.2f\n\n", 
           tree_size(root_double), tree_sum(root_double), 
           tree_height(root_double), avg_height);
    
    printf("=== УДАЛЕНИЕ ВЕРШИН ИЗ ДЕРЕВА ===\n\n");
    
    Vertex* current_tree = root_recursive;
    display_tree(current_tree, "Исходное дерево");
    
    for (int i = 0; i < 10; i++) {
        int key_to_delete;
        printf("Введите ключ для удаления %d: ", i + 1);
        scanf("%d", &key_to_delete);
        
        current_tree = delete_vertex(current_tree, key_to_delete);
        
        printf("Дерево после удаления %d:\n", key_to_delete);
        printf("Inorder обход: ");
        inorder_traversal(current_tree);
        printf("\n");
        
        char title[100];
        sprintf(title, "Дерево после удаления %d", key_to_delete);
        display_tree(current_tree, title);
    }
    
    printf("=== СЛОВАРЬ КЛЮЧЕВЫХ СЛОВ ===\n\n");
    
    FILE* source_file = fopen("SSSSDP.c", "r");
    if (source_file == NULL) {
        printf("Ошибка открытия файла с исходным кодом\n");
    } else {
        fseek(source_file, 0, SEEK_END);
        long file_size = ftell(source_file);
        fseek(source_file, 0, SEEK_SET);
        
        char* source_code = (char*)malloc(file_size + 1);
        fread(source_code, 1, file_size, source_file);
        source_code[file_size] = '\0';
        fclose(source_file);
        
        KeywordNode* dictionary = NULL;
        extract_words(source_code, &dictionary);
        
        printf("Частоты ключевых слов в программе:\n");
        print_dictionary(dictionary);
        
        free(source_code);
        free_dictionary(dictionary);
    }
    
    free_tree(root_recursive);
    free_tree(root_double);
    free_tree(root_ISDP);
    
    return 0;
}