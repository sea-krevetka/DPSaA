#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct AVLVertex {
    int data;
    int height;
    int balance;
    struct AVLVertex* left;
    struct AVLVertex* right;
} AVLVertex;

// Вспомогательные функции
int max(int a, int b) {
    return (a > b) ? a : b;
}

int get_height(AVLVertex* p) {
    return p ? p->height : 0;
}

int get_balance(AVLVertex* p) {
    return p ? get_height(p->left) - get_height(p->right) : 0;
}

void update_height_balance(AVLVertex* p) {
    if (p) {
        p->height = 1 + max(get_height(p->left), get_height(p->right));
        p->balance = get_balance(p);
    }
}

AVLVertex* rotate_right(AVLVertex* y) {
    AVLVertex* x = y->left;
    AVLVertex* T2 = x->right;

    x->right = y;
    y->left = T2;

    update_height_balance(y);
    update_height_balance(x);

    return x;
}

AVLVertex* rotate_left(AVLVertex* x) {
    AVLVertex* y = x->right;
    AVLVertex* T2 = y->left;

    y->left = x;
    x->right = T2;

    update_height_balance(x);
    update_height_balance(y);

    return y;
}

AVLVertex* balance_tree(AVLVertex* p) {
    if (!p) return p;

    update_height_balance(p);
    int balance = p->balance;

    // LL случай
    if (balance > 1 && get_balance(p->left) >= 0)
        return rotate_right(p);

    // RR случай
    if (balance < -1 && get_balance(p->right) <= 0)
        return rotate_left(p);

    // LR случай
    if (balance > 1 && get_balance(p->left) < 0) {
        p->left = rotate_left(p->left);
        return rotate_right(p);
    }

    // RL случай
    if (balance < -1 && get_balance(p->right) > 0) {
        p->right = rotate_right(p->right);
        return rotate_left(p);
    }

    return p;
}

AVLVertex* add_avl(AVLVertex* p, int data, int* rotations_count) {
    if (p == NULL) {
        AVLVertex* new_vertex = (AVLVertex*)malloc(sizeof(AVLVertex));
        new_vertex->data = data;
        new_vertex->height = 1;
        new_vertex->balance = 0;
        new_vertex->left = NULL;
        new_vertex->right = NULL;
        return new_vertex;
    }

    if (data < p->data) {
        p->left = add_avl(p->left, data, rotations_count);
    } else if (data > p->data) {
        p->right = add_avl(p->right, data, rotations_count);
    } else {
        return p;
    }

    AVLVertex* balanced = balance_tree(p);
    if (balanced != p) {
        (*rotations_count)++;
    }
    
    return balanced;
}

AVLVertex* find_min_avl(AVLVertex* p) {
    while (p && p->left != NULL) {
        p = p->left;
    }
    return p;
}

AVLVertex* delete_avl(AVLVertex* root, int key, int* rotations_count) {
    if (root == NULL) return NULL;

    if (key < root->data) {
        root->left = delete_avl(root->left, key, rotations_count);
    } else if (key > root->data) {
        root->right = delete_avl(root->right, key, rotations_count);
    } else {
        if (root->left == NULL) {
            AVLVertex* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            AVLVertex* temp = root->left;
            free(root);
            return temp;
        } else {
            AVLVertex* temp = find_min_avl(root->right);
            root->data = temp->data;
            root->right = delete_avl(root->right, temp->data, rotations_count);
        }
    }

    AVLVertex* balanced = balance_tree(root);
    if (balanced != root) {
        (*rotations_count)++;
    }
    
    return balanced;
}

void inorder_traversal_avl(AVLVertex* root) {
    if (root != NULL) {
        inorder_traversal_avl(root->left);
        printf("%d ", root->data);
        inorder_traversal_avl(root->right);
    }
}

int tree_size_avl(AVLVertex* root) {
    if (root == NULL) return 0;
    return 1 + tree_size_avl(root->left) + tree_size_avl(root->right);
}

int tree_sum_avl(AVLVertex* root) {
    if (root == NULL) return 0;
    return root->data + tree_sum_avl(root->left) + tree_sum_avl(root->right);
}

int tree_height_avl(AVLVertex* root) {
    if (root == NULL) return 0;
    return root->height;
}

void calculate_average_height_avl(AVLVertex* root, int level, int* total_path, int* total_nodes) {
    if (root == NULL) return;
    
    (*total_path) += level;
    (*total_nodes) += 1;
    
    calculate_average_height_avl(root->left, level + 1, total_path, total_nodes);
    calculate_average_height_avl(root->right, level + 1, total_path, total_nodes);
}

void generate_dot_avl(AVLVertex* root, FILE* file) {
    if (root == NULL) return;
    
    if (root->left != NULL) {
        fprintf(file, "    %d -> %d [label=\"L\"];\n", root->data, root->left->data);
        generate_dot_avl(root->left, file);
    }
    
    if (root->right != NULL) {
        fprintf(file, "    %d -> %d [label=\"R\"];\n", root->data, root->right->data);
        generate_dot_avl(root->right, file);
    }
}

void add_nodes_avl(AVLVertex* r, FILE* file) {
    if (r == NULL) return;
    fprintf(file, "    %d [label=\"%d\\nH:%d B:%d\"];\n", 
            r->data, r->data, r->height, r->balance);
    add_nodes_avl(r->left, file);
    add_nodes_avl(r->right, file);
}

void save_avl_tree_to_dot(AVLVertex* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }
    
    fprintf(file, "digraph AVLTree {\n");
    fprintf(file, "    node [shape=circle];\n");
    
    add_nodes_avl(root, file);
    generate_dot_avl(root, file);
    
    fprintf(file, "}\n");
    fclose(file);
}

void display_avl_tree(AVLVertex* root, const char* title) {
    char filename[100];
    static int counter = 0;
    sprintf(filename, "avl_tree_%d.dot", counter);
    
    save_avl_tree_to_dot(root, filename);
    printf("%s сохранен в %s\n", title, filename);
    
    char command[200];
    sprintf(command, "dot -Tpng %s -o avl_tree_%d.png", filename, counter);
    system(command);
    printf("Изображение сохранено в avl_tree_%d.png\n\n", counter);
    
    counter++;
}

typedef struct Vertex {
    int data;
    struct Vertex* left;
    struct Vertex* right;
} Vertex;

Vertex* build_ISDP(int arr[], int L, int R) {
    if (L > R) return NULL;
    
    int m = (L + R) / 2;
    Vertex* p = (Vertex*)malloc(sizeof(Vertex));
    p->data = arr[m];
    p->left = build_ISDP(arr, L, m - 1);
    p->right = build_ISDP(arr, m + 1, R);
    return p;
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

void free_avl_tree(AVLVertex* root) {
    if (root != NULL) {
        free_avl_tree(root->left);
        free_avl_tree(root->right);
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

void display_menu() {
    printf("\n=== МЕНЮ УПРАВЛЕНИЯ АВЛ-ДЕРЕВОМ ===\n");
    printf("1 - Добавить вершину\n");
    printf("2 - Удалить вершину\n");
    printf("0 - Выход\n");
    printf("Выберите действие: ");
}

void interactive_avl_tree(AVLVertex** root) {
    int choice;
    int value;
    int rotations_count = 0;
    
    do {
        display_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Введите значение для добавления: ");
                scanf("%d", &value);
                *root = add_avl(*root, value, &rotations_count);
                printf("Вершина %d добавлена в дерево.\n", value);
                printf("Inorder обход: ");
                inorder_traversal_avl(*root);
                printf("\n");
                display_avl_tree(*root, "АВЛ-дерево после добавления");
                break;
                
            case 2:
                printf("Введите значение для удаления: ");
                scanf("%d", &value);
                *root = delete_avl(*root, value, &rotations_count);
                printf("Вершина %d удалена из дерева.\n", value);
                printf("Inorder обход: ");
                inorder_traversal_avl(*root);
                printf("\n");
                display_avl_tree(*root, "АВЛ-дерево после удаления");
                break;
                
            case 3:
                printf("Inorder обход дерева: ");
                inorder_traversal_avl(*root);
                printf("\n");
                break;
                
            case 4:
                printf("Характеристики АВЛ-дерева:\n");
                printf("Размер: %d\n", tree_size_avl(*root));
                printf("Контрольная сумма: %d\n", tree_sum_avl(*root));
                printf("Высота: %d\n", tree_height_avl(*root));
                
                int total_path = 0, total_nodes = 0;
                calculate_average_height_avl(*root, 1, &total_path, &total_nodes);
                printf("Средняя высота: %.2f\n", (float)total_path / total_nodes);
                break;
                
            case 5:
                display_avl_tree(*root, "Текущее АВЛ-дерево");
                break;
                
            case 0:
                printf("Выход из меню управления.\n");
                break;
                
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
                break;
        }
        
    } while (choice != 0);
}

int main() {
    system("chcp 65001 > nul");
    srand(time(NULL));
    
    const int n = 100;
    const int max_value = 10000;
    int sequence[n];
    
    printf("=== ПОСТРОЕНИЕ И АНАЛИЗ АВЛ-ДЕРЕВА ===\n\n");
    
    printf("Генерация последовательности из %d уникальных чисел...\n", n);
    generate_unique_random_sequence(sequence, n, max_value);
    
    printf("Построение АВЛ-дерева...\n");
    AVLVertex* avl_root = NULL;
    int rotations_count = 0;
    
    for (int i = 0; i < n; i++) {
        avl_root = add_avl(avl_root, sequence[i], &rotations_count);
    }
    
    printf("Количество поворотов при построении: %d\n", rotations_count);
    printf("Соотношение поворотов/включений: %.2f\n\n", (float)rotations_count / n);
    
    printf("Обход АВЛ-дерева слева направо:\n");
    inorder_traversal_avl(avl_root);
    printf("\n\n");
    
    printf("Построение ИСДП для сравнения...\n");
    int sorted_sequence[n];
    for (int i = 0; i < n; i++) {
        sorted_sequence[i] = sequence[i];
    }
    quicksort(sorted_sequence, 0, n - 1);
    Vertex* isdp_root = build_ISDP(sorted_sequence, 0, n - 1);
    
    printf("=== СРАВНИТЕЛЬНАЯ ТАБЛИЦА ===\n\n");
    
    int total_path, total_nodes;
    float avg_height;
    
    printf("n=%d\tРазмер\tКонтр.сумма\tВысота\tСредн.высота\n", n);
    
    total_path = 0; total_nodes = 0;
    calculate_average_height(isdp_root, 1, &total_path, &total_nodes);
    avg_height = (float)total_path / total_nodes;
    
    printf("ИСДП\t%d\t%d\t\t%d\t%.2f\n", 
           tree_size(isdp_root), tree_sum(isdp_root), 
           tree_height(isdp_root), avg_height);
    
    total_path = 0; total_nodes = 0;
    calculate_average_height_avl(avl_root, 1, &total_path, &total_nodes);
    avg_height = (float)total_path / total_nodes;
    
    printf("АВЛ\t%d\t%d\t\t%d\t%.2f\n\n", 
           tree_size_avl(avl_root), tree_sum_avl(avl_root), 
           tree_height_avl(avl_root), avg_height);
    
    printf("=== ГРАФИЧЕСКОЕ ПРЕДСТАВЛЕНИЕ ===\n\n");
    display_avl_tree(avl_root, "АВЛ-дерево");

    interactive_avl_tree(&avl_root);
    printf("\n=== УДАЛЕНИЕ ВЕРШИН ИЗ АВЛ-ДЕРЕВА ===\n\n");
    
    int delete_rotations_count = 0;
    for (int i = 0; i < 10; i++) {
        int key_to_delete;
        printf("Введите ключ для удаления %d: ", i + 1);
        scanf("%d", &key_to_delete);
        
        avl_root = delete_avl(avl_root, key_to_delete, &delete_rotations_count);
        
        printf("Дерево после удаления %d:\n", key_to_delete);
        printf("Inorder обход: ");
        inorder_traversal_avl(avl_root);
        printf("\n");
        
        char title[100];
        sprintf(title, "АВЛ-дерево после удаления %d", key_to_delete);
        display_avl_tree(avl_root, title);
    }
    
    printf("Количество поворотов при удалении: %d\n", delete_rotations_count);
    printf("Соотношение поворотов/удалений: %.2f\n\n", (float)delete_rotations_count / 10);
    
    printf("=== ЭКСПЕРИМЕНТАЛЬНАЯ ПРОВЕРКА ===\n\n");
    
    AVLVertex* test_avl = NULL;
    int test_rotations = 0;
    int test_sequence[50];
    
    generate_unique_random_sequence(test_sequence, 50, 5000);
    
    for (int i = 0; i < 50; i++) {
        test_avl = add_avl(test_avl, test_sequence[i], &test_rotations);
    }
    
    printf("Добавление 50 вершин:\n");
    printf("Количество поворотов: %d\n", test_rotations);
    printf("Соотношение поворотов/включений: %.2f\n", (float)test_rotations / 50);
    
    int test_delete_rotations = 0;
    for (int i = 0; i < 10; i++) {
        int random_index = rand() % 50;
        test_avl = delete_avl(test_avl, test_sequence[random_index], &test_delete_rotations);
    }
    
    printf("\nУдаление 10 вершин:\n");
    printf("Количество поворотов: %d\n", test_delete_rotations);
    printf("Соотношение поворотов/удалений: %.2f\n", (float)test_delete_rotations / 10);
    
    free_tree(isdp_root);
    free_avl_tree(avl_root);
    free_avl_tree(test_avl);
    
    return 0;
}