#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOLS 4096
#define MAX_CODE_LENGTH 1024
#define MAX_UTF8_BYTES 5

typedef struct {
    char symbol[MAX_UTF8_BYTES];
    double probability;
    char code[MAX_CODE_LENGTH];
    int length;
    double Q; /* Gilbert-Moore position */
} GMCode;

typedef struct TreeNode {
    char symbol[MAX_UTF8_BYTES];
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

int isPrintableUTF8(const char *s) {
    unsigned char b = (unsigned char)s[0];
    if (b >= 0x20 && b != 0x7F) return 1;
    return 0;
}

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
        /* We no longer need an index array; keep code order[] as temporary */
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

double computeEntropy(GMCode *codes, int count) {
    double entropy = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].probability > 0) entropy -= codes[i].probability * log2(codes[i].probability);
    return entropy;
}

double computeAverageLength(GMCode *codes, int count) {
    double avg = 0.0;
    for (int i = 0; i < count; ++i) avg += codes[i].probability * codes[i].length;
    return avg;
}

double computeKraftSum(GMCode *codes, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].length > 0) sum += pow(2.0, -codes[i].length);
    return sum;
}

/* Convert fraction [0,1) to binary bits and write out 'len' bits into out buffer */
void fracToBinary(double frac, int len, char *out) {
    for (int i = 0; i < len; ++i) {
        frac *= 2.0;
        if (frac >= 1.0) { out[i] = '1'; frac -= 1.0; }
        else out[i] = '0';
    }
    out[len] = '\0';
}

/* Build Gilbert–Moore codes based on Qi and li = ceil(-log2(pi)) + 1 */
void buildGilbertMooreCodes(GMCode *codes, int count) {
    /* Compute Q_i = sum_{j < i} p_j + p_i / 2; symbols assumed to be ordered by alphabet */
    double cumulative = 0.0;
    for (int i = 0; i < count; ++i) {
        codes[i].Q = cumulative + codes[i].probability / 2.0;
        cumulative += codes[i].probability;
    }
    /* Now compute li and code = first li bits of Q_i */
    for (int i = 0; i < count; ++i) {
        if (codes[i].probability <= 0) { codes[i].length = 0; codes[i].code[0] = '\0'; continue; }
        int li = (int)ceil(-log2(codes[i].probability)) + 1;
        if (li < 1) li = 1;
        if (li > MAX_CODE_LENGTH-1) li = MAX_CODE_LENGTH-1;
        codes[i].length = li;
        /* Only fractional part is used; ensure Q in [0,1) */
        double q = codes[i].Q - floor(codes[i].Q);
        fracToBinary(q, li, codes[i].code);
    }
}

void appendBit(GMCode *codes, int L, int R, char bit) { /* not used but kept for compatibility */ }

/* Build tree from codes similar to others */
TreeNode *buildTreeFromCodes(GMCode *codes, int count) {
    TreeNode *root = (TreeNode*)malloc(sizeof(TreeNode));
    root->symbol[0] = '\0'; root->left = root->right = NULL;
    for (int i = 0; i < count; ++i) {
        TreeNode *cur = root;
        const char *code = codes[i].code;
        for (int j = 0; code[j] != '\0'; ++j) {
            if (code[j] == '0') {
                if (!cur->left) {
                    cur->left = (TreeNode*)malloc(sizeof(TreeNode)); cur->left->symbol[0] = '\0'; cur->left->left = cur->left->right = NULL;
                }
                cur = cur->left;
            } else {
                if (!cur->right) {
                    cur->right = (TreeNode*)malloc(sizeof(TreeNode)); cur->right->symbol[0] = '\0'; cur->right->left = cur->right->right = NULL;
                }
                cur = cur->right;
            }
        }
        strcpy(cur->symbol, codes[i].symbol);
    }
    return root;
}

void freeTree(TreeNode *root) { if (!root) return; freeTree(root->left); freeTree(root->right); free(root); }

void printTree(TreeNode *root, int level) {
    if (!root) return;
    for (int i = 0; i < level; ++i) printf("  ");
    if (root->symbol[0] != '\0') printf("'%s'\n", root->symbol); else printf("Node\n");
    printTree(root->left, level+1);
    printTree(root->right, level+1);
}

void writeGraphviz(GMCode *codes, int count, const char *dotFilename) {
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
            if (isPrintableUTF8(sym) && sym[0] != '\\' && sym[0] != '"') snprintf(nodes[cur].label, sizeof(nodes[cur].label), "'%s'\\n%.6f", sym, codes[i].probability);
            else { char hexbuf[64] = {0}; int hp = 0; for (size_t bi = 0; bi < strlen(sym) && hp < (int)sizeof(hexbuf)-5; ++bi) { hp += snprintf(hexbuf+hp, sizeof(hexbuf)-hp, "0x%02X", (unsigned char)sym[bi]); if (bi+1 < strlen(sym)) hp += snprintf(hexbuf+hp, sizeof(hexbuf)-hp, " "); } snprintf(nodes[cur].label, sizeof(nodes[cur].label), "%s\\n%.6f", hexbuf, codes[i].probability); }
        }
    }
    FILE *f = fopen(dotFilename, "w"); if (!f) { free(nodes); return; }
    fprintf(f, "digraph GMTree {\n"); fprintf(f, "  node [shape=circle, fontname=\"Helvetica\"];\n");
    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].label[0] != '\0') {
            char esc[256]; int k=0;
            for (int p = 0; nodes[i].label[p] != '\0' && k < (int)sizeof(esc)-2; ++p) { if (nodes[i].label[p] == '"') esc[k++] = '\\', esc[k++] = '"'; else esc[k++] = nodes[i].label[p]; }
            esc[k] = '\0'; fprintf(f, "  n%d [shape=box, label=\"%s\"];\n", i, esc);
        } else fprintf(f, "  n%d [label=\"\"];\n", i);
    }
    for (int i = 0; i < nodeCount; ++i) { if (nodes[i].left != -1) fprintf(f, "  n%d -> n%d [label=\"0\"];\n", i, nodes[i].left); if (nodes[i].right != -1) fprintf(f, "  n%d -> n%d [label=\"1\"];\n", i, nodes[i].right); }
    fprintf(f, "}\n"); fclose(f);
    char cmd[512]; snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o gilbert_moore_tree.png", dotFilename); system(cmd);
    free(nodes);
}

void encodeText(const char *text, GMCode *codes, int codeCount, char *encoded) {
    encoded[0] = '\0'; int i = 0;
    while (text[i] != '\0') {
        unsigned char b = (unsigned char)text[i]; int seqlen = 1;
        if ((b & 0x80) == 0x00) seqlen = 1; else if ((b & 0xE0) == 0xC0) seqlen = 2; else if ((b & 0xF0) == 0xE0) seqlen = 3; else if ((b & 0xF8) == 0xF0) seqlen = 4; else seqlen = 1;
        char sym[MAX_UTF8_BYTES]; int k; for (k = 0; k < seqlen && text[i+k] != '\0' && k < MAX_UTF8_BYTES-1; ++k) sym[k] = text[i+k]; sym[k] = '\0';
        for (int j = 0; j < codeCount; ++j) if (strcmp(codes[j].symbol, sym) == 0) { strcat(encoded, codes[j].code); break; }
        i += seqlen;
    }
}

void decodeBitString(TreeNode *root, const char *encoded, char *decoded, int maxlen) {
    if (!root) { decoded[0] = '\0'; return; }
    decoded[0] = '\0'; int di = 0; TreeNode *cur = root; int e = 0; while (encoded[e] != '\0' && di + 4 < maxlen) {
        if (encoded[e] == '0') cur = cur->left; else cur = cur->right;
        if (!cur) break;
        if (cur->symbol[0] != '\0') {
            int symlen = (int)strlen(cur->symbol);
            if (di + symlen >= maxlen) break;
            for (int s = 0; s < symlen; ++s) decoded[di++] = cur->symbol[s]; decoded[di] = '\0'; cur = root;
        }
        ++e;
    }
}

/* Comparison function for qsort: compare symbol lexicographically */
int cmpGM(const void *a, const void *b) {
    const GMCode *A = (const GMCode*)a;
    const GMCode *B = (const GMCode*)b;
    return strcmp(A->symbol, B->symbol);
}

int main(int argc, char **argv) {
    system("chcp 65001 > nul");
    const char *filename = (argc > 1) ? argv[1] : "file.txt";
    char symbols[MAX_SYMBOLS][MAX_UTF8_BYTES]; int counts[MAX_SYMBOLS]; int unique = 0; int total = 0;
    countFrequenciesUTF8(filename, symbols, counts, &unique, &total);
    if (unique <= 0) { printf("Файл пуст или символы не найдены\n"); return 0; }

    GMCode *codes = (GMCode*)malloc(unique * sizeof(GMCode));
    for (int i = 0; i < unique; ++i) { strncpy(codes[i].symbol, symbols[i], MAX_UTF8_BYTES-1); codes[i].symbol[MAX_UTF8_BYTES-1] = '\0'; codes[i].probability = (double)counts[i] / total; codes[i].code[0] = '\0'; codes[i].length = 0; codes[i].Q = 0.0; }

    /* Normalize and print sums */
    double p_sum = 0.0;
    for (int i = 0; i < unique; ++i) p_sum += codes[i].probability;
    printf("Сумма вероятностей (до нормализации): %.15f\n", p_sum);
    if (p_sum > 0.0) {
        for (int i = 0; i < unique; ++i) codes[i].probability /= p_sum;
        double tmp = 0.0; for (int i = 0; i < unique - 1; ++i) tmp += codes[i].probability; codes[unique - 1].probability = 1.0 - tmp;
    }
    double sum_after = 0.0; for (int i = 0; i < unique; ++i) sum_after += codes[i].probability;
    printf("Сумма вероятностей (после нормализации): %.15f\n", sum_after);
    if (fabs(sum_after - 1.0) > 1e-12) printf("\nВнимание: сумма вероятностей отличается от 1 (%.15f).\n", sum_after);

    /* Gilbert-Moore requires alphabetic order; we'll sort lexicographically by symbol to get an alphabet order */
    /* Temporary array to sort codes in alphabetic order */
    /* build a temp array of GMCode in lexicographic order */
    GMCode *order = (GMCode*)malloc(unique * sizeof(GMCode));
    /* sort by symbol content using insertion sort because we need stable mapping to Qs */
    for (int i = 0; i < unique; ++i) order[i] = codes[i];
    /* simple qsort using strcmp on symbol */
    qsort(order, unique, sizeof(GMCode), cmpGM);

    buildGilbertMooreCodes(order, unique);
    /* Now we want to map codes back into original codes[] by symbol match */
    for (int i = 0; i < unique; ++i) {
        for (int j = 0; j < unique; ++j) {
            if (strcmp(order[i].symbol, codes[j].symbol) == 0) {
                strncpy(codes[j].code, order[i].code, MAX_CODE_LENGTH-1); codes[j].code[MAX_CODE_LENGTH-1] = '\0'; codes[j].length = order[i].length; codes[j].Q = order[i].Q; break;
            }
        }
    }

    printf("Код Гилберта-Мура\n"); printf("Символ\tВероятность\tКодовое слово\tДлина\n");
    for (int i = 0; i < unique; ++i) {
        if (strlen(codes[i].symbol) == 1) {
            unsigned char b = (unsigned char)codes[i].symbol[0];
            if (b == '\n') { printf("'\\n'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
            if (b == '\r') { printf("'\\r'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
            if (b == '\t') { printf("'\\t'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); continue; }
        }
        if (isPrintableUTF8(codes[i].symbol)) printf("'%s'\t%10.6f\t%-20s\t%3d\n", codes[i].symbol, codes[i].probability, codes[i].code, codes[i].length);
        else { for (size_t bi = 0; bi < strlen(codes[i].symbol); ++bi) { if (bi) printf(" "); printf("0x%02X", (unsigned char)codes[i].symbol[bi]); } printf("\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length); }
    }

    double kraft_sum = computeKraftSum(codes, unique);
    double entropy = computeEntropy(codes, unique);
    double avg_len = computeAverageLength(codes, unique);
    double redundancy = avg_len - entropy;
    printf("\nНеравенство Крафта: sum 2^{-l_i} = %.15f (%s)\n", kraft_sum, kraft_sum <= 1.0 ? "Выполнено" : "Не выполнено");
    printf("Энтропия: %.6f\nСредняя длина кодового слова: %.6f\nИзбыточность: %.6f\n\n", entropy, avg_len, redundancy);

    /* Sample text (first 100 bytes) */
    char sample_text[101]; FILE *file = fopen(filename, "rb"); if (file) { size_t r = fread(sample_text, 1, 100, file); sample_text[r] = '\0'; fclose(file); } else strcpy(sample_text, "Пример текста для кодирования");
    char encoded[100 * MAX_CODE_LENGTH]; encoded[0] = '\0'; encodeText(sample_text, codes, unique, encoded);
    int original_bits = 100 * 8; int encoded_bits = (int)strlen(encoded); double comp_ratio = encoded_bits > 0 ? (double)original_bits / encoded_bits : 0.0;
    printf("Исходный текст (100 символов): %s\n", sample_text);
    printf("Закодированная последовательность: %s\n", encoded);
    printf("Длина закодированной последовательности: %d бит\nКоэффициент сжатия: %.6f\n\n", encoded_bits, comp_ratio);

    writeGraphviz(codes, unique, "gilbert_moore_tree.dot");
    TreeNode *root = buildTreeFromCodes(codes, unique);
    printf("Дерево кодов Гилберта-Мура:\n"); printTree(root, 0);

    /* decode and verify */
    char decoded[1024]; decodeBitString(root, encoded, decoded, sizeof(decoded));
    printf("Декодированный текст: %s\n", decoded);
    printf("Декодирование %s\n", strcmp(sample_text, decoded) == 0 ? "успешно" : "не совпадает");

    freeTree(root);
    free(order); free(codes);
    return 0;
}
