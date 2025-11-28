#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOLS 4096
#define MAX_CODE_LENGTH 64
#define MAX_UTF8_BYTES 5

typedef struct {
    char symbol[MAX_UTF8_BYTES];
    double probability;
    char code[MAX_CODE_LENGTH];
    int length;
} FanoCode;

typedef struct TreeNode {
    char symbol[MAX_UTF8_BYTES];
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* Utility functions copied/adapted from shennon.c */
int isPrintableUTF8(const char *s) {
    unsigned char b = (unsigned char)s[0];
    if (b >= 0x20 && b != 0x7F) return 1;
    return 0;
}

int compareDesc(const void *a, const void *b) {
    const FanoCode *A = (const FanoCode*)a;
    const FanoCode *B = (const FanoCode*)b;
    if (A->probability > B->probability) return -1;
    if (A->probability < B->probability) return 1;
    return 0;
}

/* Count UTF-8 codepoint occurrences from file (copied/adapted) */
void countFrequenciesUTF8(const char *filename, char symbols[][MAX_UTF8_BYTES], int *counts, int *unique, int *total) {
    FILE *file = fopen(filename, "rb");
    if (!file) { perror("Не удалось открыть файл"); exit(1); }

    fseek(file, 0, SEEK_END);
    long filelen = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *buf = (unsigned char*)malloc(filelen > 0 ? filelen : 1);
    if (!buf) { fclose(file); perror("malloc"); exit(1); }
    size_t read = fread(buf, 1, filelen, file);
    (void)read;
    fclose(file);

    for (int i = 0; i < MAX_SYMBOLS; ++i) counts[i] = 0;
    *unique = 0;
    *total = 0;

    long i = 0;
    while (i < filelen) {
        unsigned char b = buf[i];
        int seqlen = 1;
        if ((b & 0x80) == 0x00) seqlen = 1;
        else if ((b & 0xE0) == 0xC0) seqlen = 2;
        else if ((b & 0xF0) == 0xE0) seqlen = 3;
        else if ((b & 0xF8) == 0xF0) seqlen = 4;
        else seqlen = 1;

        if (i + seqlen > filelen) seqlen = (int)(filelen - i);

        char sym[MAX_UTF8_BYTES];
        int k;
        for (k = 0; k < seqlen && k < MAX_UTF8_BYTES-1; ++k) sym[k] = (char)buf[i+k];
        sym[k] = '\0';

        int found = -1;
        for (int s = 0; s < *unique; ++s) {
            if (strcmp(symbols[s], sym) == 0) { found = s; break; }
        }
        if (found == -1) {
            if (*unique >= MAX_SYMBOLS) break;
            strcpy(symbols[*unique], sym);
            counts[*unique] = 1;
            (*unique)++;
        } else {
            counts[found]++;
        }

        (*total)++;
        i += seqlen;
    }

    free(buf);
}

/* Compute entropy, average length, kraft sum */
double computeEntropy(FanoCode *codes, int count) {
    double entropy = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].probability > 0) entropy -= codes[i].probability * log2(codes[i].probability);
    return entropy;
}

double computeAverageLength(FanoCode *codes, int count) {
    double avg = 0.0;
    for (int i = 0; i < count; ++i) avg += codes[i].probability * codes[i].length;
    return avg;
}

double computeKraftSum(FanoCode *codes, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].length > 0) sum += pow(2.0, -codes[i].length);
    return sum;
}

/* Shannon-Fano median index (Med) */
int findMedianIndex(FanoCode *codes, int L, int R) {
    if (L >= R) return L;
    double total = 0.0;
    for (int i = L; i <= R; ++i) total += codes[i].probability;
    double acc = 0.0;
    int best = L;
    double bestDiff = 1e300;
    for (int i = L; i < R; ++i) {
        acc += codes[i].probability;
        double right = total - acc;
        double diff = fabs(acc - right);
        if (diff < bestDiff) { bestDiff = diff; best = i; }
    }
    return best;
}

/* Append a bit to codes in range [L..R] */
void appendBit(FanoCode *codes, int L, int R, char bit) {
    for (int i = L; i <= R; ++i) {
        size_t len = strlen(codes[i].code);
        if (len + 1 < MAX_CODE_LENGTH) {
            codes[i].code[len] = bit;
            codes[i].code[len+1] = '\0';
            codes[i].length = (int)strlen(codes[i].code);
        }
    }
}

/* Recursive Fano code: split and append bits */
void fanoRecursive(FanoCode *codes, int L, int R) {
    if (L >= R) return;
    int m = findMedianIndex(codes, L, R);
    appendBit(codes, L, m, '0');
    appendBit(codes, m+1, R, '1');
    fanoRecursive(codes, L, m);
    fanoRecursive(codes, m+1, R);
}

/* Graphviz tree writer based on codes (adapted from shennon.c) */
void writeGraphviz(FanoCode *codes, int count, const char *dotFilename) {
    const int MAX_NODES = 16384;
    typedef struct { int left, right; char label[128]; } DotNode;
    DotNode *nodes = (DotNode*)malloc(MAX_NODES * sizeof(DotNode));
    if (!nodes) return;
    for (int i = 0; i < MAX_NODES; ++i) { nodes[i].left = nodes[i].right = -1; nodes[i].label[0] = '\0'; }

    int nodeCount = 1;

    for (int i = 0; i < count; ++i) {
        int cur = 0;
        const char *code = codes[i].code;
        for (int j = 0; code[j] != '\0'; ++j) {
            if (code[j] == '0') {
                if (nodes[cur].left == -1) { if (nodeCount >= MAX_NODES) break; nodes[cur].left = nodeCount++; }
                cur = nodes[cur].left;
            } else {
                if (nodes[cur].right == -1) { if (nodeCount >= MAX_NODES) break; nodes[cur].right = nodeCount++; }
                cur = nodes[cur].right;
            }
        }
        if (codes[i].probability > 0.0) {
            const char *sym = codes[i].symbol;
            if (isPrintableUTF8(sym) && sym[0] != '\\' && sym[0] != '"') {
                snprintf(nodes[cur].label, sizeof(nodes[cur].label), "'%s'\\n%.6f", sym, codes[i].probability);
            } else {
                char hexbuf[64] = {0};
                int hp = 0;
                for (size_t bi = 0; bi < strlen(sym) && hp < (int)sizeof(hexbuf)-5; ++bi) {
                    hp += snprintf(hexbuf+hp, sizeof(hexbuf)-hp, "0x%02X", (unsigned char)sym[bi]);
                    if (bi+1 < strlen(sym)) hp += snprintf(hexbuf+hp, sizeof(hexbuf)-hp, " ");
                }
                snprintf(nodes[cur].label, sizeof(nodes[cur].label), "%s\\n%.6f", hexbuf, codes[i].probability);
            }
        }
    }

    FILE *f = fopen(dotFilename, "w");
    if (!f) { free(nodes); return; }
    fprintf(f, "digraph FanoTree {\n");
    fprintf(f, "  node [shape=circle, fontname=\"Helvetica\"];\n");

    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].label[0] != '\0') {
            char esc[256]; int k=0;
            for (int p = 0; nodes[i].label[p] != '\0' && k < (int)sizeof(esc)-2; ++p) {
                if (nodes[i].label[p] == '"') esc[k++] = '\\', esc[k++] = '"'; else esc[k++] = nodes[i].label[p];
            }
            esc[k] = '\0';
            fprintf(f, "  n%d [shape=box, label=\"%s\"];\n", i, esc);
        } else fprintf(f, "  n%d [label=\"\"];\n", i);
    }
    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].left != -1) fprintf(f, "  n%d -> n%d [label=\"0\"];\n", i, nodes[i].left);
        if (nodes[i].right != -1) fprintf(f, "  n%d -> n%d [label=\"1\"];\n", i, nodes[i].right);
    }
    fprintf(f, "}\n");
    fclose(f);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o fano_tree.png", dotFilename);
    system(cmd);
    free(nodes);
}

void printTree(TreeNode *root, int level) {
    if (!root) return;
    for (int i = 0; i < level; ++i) printf("  ");
    if (root->symbol[0] != '\0') printf("'%s'\n", root->symbol); else printf("Node\n");
    printTree(root->left, level+1);
    printTree(root->right, level+1);
}

TreeNode* buildTreeFromCodes(FanoCode *codes, int count) {
    TreeNode *root = (TreeNode*)malloc(sizeof(TreeNode));
    root->symbol[0] = '\0'; root->left = root->right = NULL;
    for (int i = 0; i < count; ++i) {
        TreeNode *cur = root;
        const char *code = codes[i].code;
        for (int j = 0; code[j] != '\0'; ++j) {
            if (code[j] == '0') {
                if (!cur->left) {
                    cur->left = (TreeNode*)malloc(sizeof(TreeNode));
                    cur->left->symbol[0] = '\0'; cur->left->left = cur->left->right = NULL;
                }
                cur = cur->left;
            } else {
                if (!cur->right) {
                    cur->right = (TreeNode*)malloc(sizeof(TreeNode));
                    cur->right->symbol[0] = '\0'; cur->right->left = cur->right->right = NULL;
                }
                cur = cur->right;
            }
        }
        strcpy(cur->symbol, codes[i].symbol);
    }
    return root;
}

void freeTree(TreeNode *root) {
    if (!root) return;
    freeTree(root->left); freeTree(root->right); free(root);
}

void encodeText(const char *text, FanoCode *codes, int codeCount, char *encoded) {
    encoded[0] = '\0';
    int i = 0;
    while (text[i] != '\0') {
        unsigned char b = (unsigned char)text[i];
        int seqlen = 1;
        if ((b & 0x80) == 0x00) seqlen = 1;
        else if ((b & 0xE0) == 0xC0) seqlen = 2;
        else if ((b & 0xF0) == 0xE0) seqlen = 3;
        else if ((b & 0xF8) == 0xF0) seqlen = 4;
        else seqlen = 1;

        char sym[MAX_UTF8_BYTES];
        int k; for (k = 0; k < seqlen && text[i+k] != '\0' && k < MAX_UTF8_BYTES-1; ++k) sym[k] = text[i+k]; sym[k] = '\0';
        for (int j = 0; j < codeCount; ++j) if (strcmp(codes[j].symbol, sym) == 0) { strcat(encoded, codes[j].code); break; }
        i += seqlen;
    }
}

int main(int argc, char **argv) {
    system("chcp 65001 > nul");
    const char *filename = (argc > 1) ? argv[1] : "file.txt";

    char symbols[MAX_SYMBOLS][MAX_UTF8_BYTES];
    int counts[MAX_SYMBOLS];
    int uniqueSymbols = 0; int totalSymbols = 0;
    countFrequenciesUTF8(filename, symbols, counts, &uniqueSymbols, &totalSymbols);
    if (uniqueSymbols <= 0) { printf("Файл пуст или символы не найдены\n"); return 0; }

    FanoCode *codes = (FanoCode*)malloc(uniqueSymbols * sizeof(FanoCode));
    for (int i = 0; i < uniqueSymbols; ++i) {
        strncpy(codes[i].symbol, symbols[i], MAX_UTF8_BYTES-1);
        codes[i].symbol[MAX_UTF8_BYTES-1] = '\0';
        codes[i].probability = (double)counts[i] / totalSymbols;
        codes[i].code[0] = '\0'; codes[i].length = 0;
    }

    /* Normalize just in case and print sum checks */
    double p_sum = 0.0;
    for (int i = 0; i < uniqueSymbols; ++i) p_sum += codes[i].probability;
    printf("Сумма вероятностей (до нормализации): %.15f\n", p_sum);
    if (p_sum > 0.0) {
        for (int i = 0; i < uniqueSymbols; ++i) codes[i].probability /= p_sum;
        /* ensure the sum is exactly 1 by fixing last element */
        double tmp = 0.0;
        for (int i = 0; i < uniqueSymbols - 1; ++i) tmp += codes[i].probability;
        codes[uniqueSymbols - 1].probability = 1.0 - tmp;
    }
    double sum_after = 0.0; for (int i = 0; i < uniqueSymbols; ++i) sum_after += codes[i].probability;
    printf("Сумма вероятностей (после нормализации): %.15f\n", sum_after);
    if (fabs(sum_after - 1.0) > 1e-12) printf("\nВнимание: сумма вероятностей отличается от 1 (%.15f).\n", sum_after);

    qsort(codes, uniqueSymbols, sizeof(FanoCode), compareDesc);

    fanoRecursive(codes, 0, uniqueSymbols-1);

    printf("Код Фано\n");
    printf("Символ\tВероятность\tКодовое слово\tДлина\n");
    for (int i = 0; i < uniqueSymbols; ++i) {
        if (strlen(codes[i].symbol) == 1) {
            unsigned char b = (unsigned char)codes[i].symbol[0];
            if (b == '\n') { printf("'\\n'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
            if (b == '\r') { printf("'\\r'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
            if (b == '\t') { printf("'\\t'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
        }
        if (isPrintableUTF8(codes[i].symbol)) printf("'%s'\t%10.6f\t%-20s\t%3d\n", codes[i].symbol, codes[i].probability, codes[i].code, codes[i].length);
        else { for (size_t bi = 0; bi < strlen(codes[i].symbol); ++bi) { if (bi) printf(" "); printf("0x%02X", (unsigned char)codes[i].symbol[bi]); } printf("\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); }
    }

    double kraft_sum = computeKraftSum(codes, uniqueSymbols);
    double entropy = computeEntropy(codes, uniqueSymbols);
    double avg_length = computeAverageLength(codes, uniqueSymbols);
    double redundancy = avg_length - entropy;
    printf("\nНеравенство Крафта: sum 2^{-l_i} = %.15f (%s)\n", kraft_sum, kraft_sum <= 1.0 ? "Выполнено" : "Не выполнено");
    printf("Энтропия: %.6f\nСредняя длина кодового слова: %.6f\nИзбыточность: %.6f\n\n", entropy, avg_length, redundancy);

    /* Sample text (first 100 bytes) */
    char sample_text[101];
    FILE *file = fopen(filename, "rb");
    if (file) { size_t r = fread(sample_text, 1, 100, file); sample_text[r] = '\0'; fclose(file); }
    else strcpy(sample_text, "Пример текста для кодирования");
    char encoded[100 * MAX_CODE_LENGTH]; encoded[0] = '\0';
    encodeText(sample_text, codes, uniqueSymbols, encoded);
    int original_bits = 100 * 8;
    int encoded_bits = (int)strlen(encoded);
    double comp_ratio = encoded_bits > 0 ? (double)original_bits / encoded_bits : 0.0;
    printf("Исходный текст (100 символов): %s\n", sample_text);
    printf("Закодированная последовательность: %s\n", encoded);
    printf("Длина закодированной последовательности: %d бит\nКоэффициент сжатия: %.6f\n\n", encoded_bits, comp_ratio);

    /* Graphviz file */
    writeGraphviz(codes, uniqueSymbols, "fano_tree.dot");

    TreeNode *root = buildTreeFromCodes(codes, uniqueSymbols);
    printf("Дерево кодов Фано:\n");
    printTree(root, 0);
    freeTree(root);

    free(codes);
    return 0;
}
