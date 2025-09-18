#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void inorder_traversal(Vertex* root) {
    if (root != NULL) {
        inorder_traversal(root->left);
        printf("%d ", root->data);
        inorder_traversal(root->right);
    }
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

void save_tree_to_file(Vertex* root, FILE* file, int parent, char* direction) {
    if (root == NULL) return;
    
    if (parent == -1) {
        fprintf(file, "%d null null\n", root->data);
    } else {
        fprintf(file, "%d %d %s\n", root->data, parent, direction);
    }
    
    save_tree_to_file(root->left, file, root->data, "left");
    save_tree_to_file(root->right, file, root->data, "right");
}

void save_all_trees_to_file(Vertex* tree1, Vertex* tree2, Vertex* tree3, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи\n");
        return;
    }
    
    fprintf(file, "=== Tree 1 (Рекурсивное СДП) ===\n");
    save_tree_to_file(tree1, file, -1, "");
    
    fprintf(file, "=== Tree 2 (Двойная косвенность) ===\n");
    save_tree_to_file(tree2, file, -1, "");
    
    fprintf(file, "=== Tree 3 (ИСДП) ===\n");
    save_tree_to_file(tree3, file, -1, "");
    
    fclose(file);
    printf("Деревья сохранены в файл: %s\n", filename);
}


int main() {
    srand(time(NULL));
    const int n = 100;
    const int max_value = 10000;
    int sequence[n];
    
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
    
    printf("СДП2\t%d\t%d\t\t%d\t%.2f\n", 
           tree_size(root_double), tree_sum(root_double), 
           tree_height(root_double), avg_height);

    save_all_trees_to_file(root_recursive, root_double, root_ISDP, "trees.txt");
    
    free_tree(root_recursive);
    free_tree(root_double);
    free_tree(root_ISDP);
    
    return 0;
}