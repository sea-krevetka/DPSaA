#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOLS 4096
#define MAX_CODE_LENGTH 1024
#define MAX_UTF8_BYTES 5

typedef struct HuffNode {
    char symbol[MAX_UTF8_BYTES];
    double probability;
    struct HuffNode *left;
    struct HuffNode *right;
    int is_leaf;
} HuffNode;

typedef struct {
    char symbol[MAX_UTF8_BYTES];
    double probability;
    char code[MAX_CODE_LENGTH];
    int length;
} HuffCode;

int isPrintableUTF8(const char *s) {
    unsigned char b = (unsigned char)s[0];
    if (b >= 0x20 && b != 0x7F) return 1;
    return 0;
}

/* Count UTF-8 codepoint occurrences from file */
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
    *unique = 0; *total = 0;

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

        char sym[MAX_UTF8_BYTES]; int k;
        for (k = 0; k < seqlen && k < MAX_UTF8_BYTES-1; ++k) sym[k] = (char)buf[i+k];
        sym[k] = '\0';

        int found = -1;
        for (int s = 0; s < *unique; ++s) if (strcmp(symbols[s], sym) == 0) { found = s; break; }
        if (found == -1) { if (*unique >= MAX_SYMBOLS) break; strcpy(symbols[*unique], sym); counts[*unique] = 1; (*unique)++; }
        else counts[found]++;

        (*total)++;
        i += seqlen;
    }

    free(buf);
}

/* Compute entropy, average length, kraft sum */
double computeEntropy(HuffCode *codes, int count) {
    double entropy = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].probability > 0) entropy -= codes[i].probability * log2(codes[i].probability);
    return entropy;
}

double computeAverageLength(HuffCode *codes, int count) {
    double avg = 0.0;
    for (int i = 0; i < count; ++i) avg += codes[i].probability * codes[i].length;
    return avg;
}

double computeKraftSum(HuffCode *codes, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; ++i) if (codes[i].length > 0) sum += pow(2.0, -codes[i].length);
    return sum;
}

/* Build Huffman tree using simple O(n^2) selection */
HuffNode* buildHuffmanTree(HuffNode **nodes, int *n) {
    if (*n == 0) return NULL;
    while (*n > 1) {
        int i_min = -1, j_min = -1;
        for (int i = 0; i < *n; ++i) {
            if (i_min == -1 || nodes[i]->probability < nodes[i_min]->probability) { i_min = i; }
        }
        for (int j = 0; j < *n; ++j) {
            if (j == i_min) continue;
            if (j_min == -1 || nodes[j]->probability < nodes[j_min]->probability) j_min = j;
        }
        int a = i_min, b = j_min;
        if (a > b) { int t=a; a=b; b=t; }
        HuffNode *newNode = (HuffNode*)malloc(sizeof(HuffNode));
        newNode->symbol[0] = '\0'; newNode->left = nodes[a]; newNode->right = nodes[b]; newNode->is_leaf = 0;
        newNode->probability = nodes[a]->probability + nodes[b]->probability;
        /* remove indices a & b and append newNode */
        for (int k = b+1; k < *n; ++k) nodes[k-1] = nodes[k];
        for (int k = a+1; k < *n-1; ++k) nodes[k-1] = nodes[k];
        nodes[*n-2] = newNode;
        *n = *n - 1;
    }
    return nodes[0];
}

/* Generate codes by traversing tree */
void generateCodesFromTree(HuffNode *root, HuffCode *codes, int *codeCount, char *prefix, int depth) {
    if (!root) return;
    if (root->is_leaf) {
        for (int i = 0; i < *codeCount; ++i) {
            if (strcmp(codes[i].symbol, root->symbol) == 0) {
                strncpy(codes[i].code, prefix, MAX_CODE_LENGTH-1); codes[i].code[MAX_CODE_LENGTH-1] = '\0';
                codes[i].length = depth; break;
            }
        }
        return;
    }
    if (root->left) {
        if (depth < MAX_CODE_LENGTH-1) { prefix[depth] = '0'; prefix[depth+1] = '\0'; generateCodesFromTree(root->left, codes, codeCount, prefix, depth+1); }
    }
    if (root->right) {
        if (depth < MAX_CODE_LENGTH-1) { prefix[depth] = '1'; prefix[depth+1] = '\0'; generateCodesFromTree(root->right, codes, codeCount, prefix, depth+1); }
    }
}

/* Write Graphviz for Huffman tree: traverse nodes and put unique ids */
void writeGraphvizFromTree(HuffNode *root, const char *dotFilename) {
    FILE *f = fopen(dotFilename, "w");
    if (!f) return;
    fprintf(f, "digraph HuffmanTree {\n");
    fprintf(f, "  node [shape=circle, fontname=\"Helvetica\"];\n");
    int id = 0;

    /* Using recursive lambda-like approach via stack */
    typedef struct { HuffNode *node; int id; } StackItem;
    StackItem stack[8192]; int sp = 0; stack[sp++] = (StackItem){root, id++};

    /* To store edges + labels, we'll use arrays */
    struct Edge { int from, to; char label; } edges[8192]; int edgeCount = 0;
    char labels[8192][64]; int labelCount = 0;
    int nodeIds[8192]; /* parallel to stack by index, but it's ok to store mapping using address -> id; for simplicity build mapping with arrays */

    /* We'll use a simple mapping from pointers to numeric id using arrays */
    HuffNode *nodesMap[8192]; int nodesCount = 0;
    nodesMap[nodesCount++] = root; int nodesIds[8192]; nodesIds[0] = 0;

    /* BFS build mapping */
    int qh = 0; HuffNode *q[8192]; q[qh++] = root; int qtail = 0;
    while (qtail < qh) {
        HuffNode *cur = q[qtail++];
        int curId = -1; for (int i = 0; i < nodesCount; ++i) if (nodesMap[i] == cur) { curId = nodesIds[i]; break; }
        if (!cur) continue;
        if (cur->left) { nodesMap[nodesCount] = cur->left; nodesIds[nodesCount] = id; nodesCount++; edges[edgeCount++] = (struct Edge){curId, id, '0'}; id++; q[qh++] = cur->left; }
        if (cur->right) { nodesMap[nodesCount] = cur->right; nodesIds[nodesCount] = id; nodesCount++; edges[edgeCount++] = (struct Edge){curId, id, '1'}; id++; q[qh++] = cur->right; }
    }

    /* Print nodes: we need to identify leaves and put label accordingly */
    for (int i = 0; i < nodesCount; ++i) {
        int nid = nodesIds[i]; HuffNode *n = nodesMap[i];
        if (n->is_leaf) {
            char esc[128] = {0};
            if (isPrintableUTF8(n->symbol)) snprintf(esc, sizeof(esc), "'%s'\\n%.6f", n->symbol, n->probability);
            else {
                char hex[64] = {0}; int hp = 0; for (size_t bi = 0; bi < strlen(n->symbol) && hp < (int)sizeof(hex)-6; ++bi) hp += snprintf(hex+hp, sizeof(hex)-hp, "0x%02X ", (unsigned char)n->symbol[bi]);
                snprintf(esc, sizeof(esc), "%s\\n%.6f", hex, n->probability);
            }
            fprintf(f, "  n%d [shape=box, label=\"%s\"];\n", nid, esc);
        } else {
            fprintf(f, "  n%d [label=\"\"];\n", nid);
        }
    }
    for (int i = 0; i < edgeCount; ++i) {
        fprintf(f, "  n%d -> n%d [label=\"%c\"];\n", edges[i].from, edges[i].to, edges[i].label);
    }
    fprintf(f, "}\n"); fclose(f);
    char cmd[512]; snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o huffman_tree.png", dotFilename); system(cmd);
}

void encodeText(const char *text, HuffCode *codes, int codeCount, char *encoded) {
    encoded[0] = '\0'; int i = 0;
    while (text[i] != '\0') {
        unsigned char b = (unsigned char)text[i]; int seqlen = 1;
        if ((b & 0x80) == 0x00) seqlen = 1; else if ((b & 0xE0) == 0xC0) seqlen = 2; else if ((b & 0xF0) == 0xE0) seqlen = 3; else if ((b & 0xF8) == 0xF0) seqlen = 4; else seqlen = 1;
        char sym[MAX_UTF8_BYTES]; int k; for (k = 0; k < seqlen && text[i+k] != '\0' && k < MAX_UTF8_BYTES-1; ++k) sym[k] = text[i+k]; sym[k] = '\0';
        for (int j = 0; j < codeCount; ++j) if (strcmp(codes[j].symbol, sym) == 0) { strcat(encoded, codes[j].code); break; }
        i += seqlen;
    }
}

/* Decode encoded bits back using Huffman tree */
void decodeBitString(HuffNode *root, const char *encoded, char *decoded, int maxlen) {
    if (!root) { decoded[0] = '\0'; return; }
    decoded[0] = '\0'; int di = 0; HuffNode *cur = root; int e = 0; while (encoded[e] != '\0' && di + 4 < maxlen) {
        if (encoded[e] == '0') cur = cur->left; else cur = cur->right;
        if (!cur) break;
        if (cur->is_leaf) {
            int symlen = (int)strlen(cur->symbol);
            if (di + symlen >= maxlen) break;
            for (int s = 0; s < symlen; ++s) decoded[di++] = cur->symbol[s];
            decoded[di] = '\0';
            cur = root;
        }
        ++e;
    }
}

/* Build Huffman codes and mapping codes[] */
int main(int argc, char **argv) {
    system("chcp 65001 > nul");
    const char *filename = (argc > 1) ? argv[1] : "file.txt";
    char symbols[MAX_SYMBOLS][MAX_UTF8_BYTES]; int counts[MAX_SYMBOLS]; int unique = 0; int total = 0;
    countFrequenciesUTF8(filename, symbols, counts, &unique, &total);
    if (unique <= 0) { printf("Файл пуст или символы не найдены\n"); return 0; }

    /* Initialize codes array */
    HuffCode *codes = (HuffCode*)malloc(unique * sizeof(HuffCode));
    for (int i = 0; i < unique; ++i) { strncpy(codes[i].symbol, symbols[i], MAX_UTF8_BYTES-1); codes[i].symbol[MAX_UTF8_BYTES-1] = '\0'; codes[i].probability = (double)counts[i] / total; codes[i].code[0] = '\0'; codes[i].length = 0; }

    /* Normalize probabilities and print sum checks */
    double p_sum = 0.0;

    
    /* Create node list */
    HuffNode *nodesList[MAX_SYMBOLS]; int nNodes = 0;
    for (int i = 0; i < unique; ++i) {
        HuffNode *nd = (HuffNode*)malloc(sizeof(HuffNode)); nd->left = nd->right = NULL; nd->is_leaf = 1; strncpy(nd->symbol, codes[i].symbol, MAX_UTF8_BYTES-1); nd->symbol[MAX_UTF8_BYTES-1] = '\0'; nd->probability = codes[i].probability; nodesList[nNodes++] = nd;
    }

    /* Build Huffman tree */
    HuffNode *root = buildHuffmanTree(nodesList, &nNodes);

    /* Generate codes by DFS */
    char prefix[MAX_CODE_LENGTH]; prefix[0] = '\0'; generateCodesFromTree(root, codes, &unique, prefix, 0);

    /* Print code table */
    printf("Код Хаффмана\n"); printf("Символ\tВероятность\tКодовое слово\tДлина\n");
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
    double sum_after = 0.0; for (int i = 0; i < unique; ++i) sum_after += codes[i].probability;
    printf("Сумма вероятностей: %.15f\n", sum_after);
    printf("\nНеравенство Крафта: sum 2^{-l_i} = %.15f (%s)\n", kraft_sum, kraft_sum <= 1.0 ? "Выполнено" : "Не выполнено");
    printf("Энтропия: %.6f\nСредняя длина кодового слова: %.6f\nИзбыточность: %.6f\n\n", entropy, avg_len, redundancy);

    /* Sample text */
    char sample_text[101]; FILE *file = fopen(filename, "rb"); if (file) { size_t r = fread(sample_text, 1, 100, file); sample_text[r] = '\0'; fclose(file); } else strcpy(sample_text, "Пример текста для кодирования");
    char encoded[100 * 32]; encoded[0] = '\0'; encodeText(sample_text, codes, unique, encoded);

    int original_bits = 100 * 8;
    int encoded_bits = (int)strlen(encoded);
    double comp_ratio = encoded_bits > 0 ? (double)original_bits / encoded_bits : 0.0;
    printf("Исходный текст (100 символов): %s\n", sample_text);
    printf("Закодированная последовательность: %s\n", encoded);
    printf("Длина закодированной последовательности: %d бит\nКоэффициент сжатия: %.6f\n\n", encoded_bits, comp_ratio);

    /* Decode encoded and compare */
    char decoded[1024]; decodeBitString(root, encoded, decoded, sizeof(decoded));
    printf("Декодированный текст (из закодированной последовательности): %s\n", decoded);
    printf("Декодирование %s\n", strcmp(sample_text, decoded) == 0 ? "успешно" : "не совпадает");

    writeGraphvizFromTree(root, "huffman_tree.dot");
    /* cleanup: free nodes */
    // Ideally, we'd free the whole tree recursively, but since OS frees after program exit, and for brevity, skip here.
    free(codes);
    return 0;
}
