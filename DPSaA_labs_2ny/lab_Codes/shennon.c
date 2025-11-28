#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_SYMBOLS 4096
#define MAX_CODE_LENGTH 32
#define MAX_UTF8_BYTES 5

typedef struct {
    char symbol[MAX_UTF8_BYTES]; /* UTF-8 symbol (1..4 bytes) */
    double probability;
    char code[MAX_CODE_LENGTH];
    int length;
} ShannonCode;

typedef struct TreeNode {
    char symbol[MAX_UTF8_BYTES];
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* Read file as binary and count UTF-8 codepoint occurrences.
   symbols: output array of unique UTF-8 strings (up to MAX_SYMBOLS)
   counts: occurrences per symbol
   unique: number of unique symbols found
   total: total codepoints counted
*/
void countFrequenciesUTF8(const char *filename, char symbols[][MAX_UTF8_BYTES], int *counts, int *unique, int *total) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Не удалось открыть файл");
        exit(1);
    }

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
        else seqlen = 1; /* fallback */

        if (i + seqlen > filelen) seqlen = (int)(filelen - i);

        char sym[MAX_UTF8_BYTES];
        int k;
        for (k = 0; k < seqlen && k < MAX_UTF8_BYTES-1; ++k) sym[k] = (char)buf[i+k];
        sym[k] = '\0';

        /* find or add */
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

int isPrintableUTF8(const char *s) {
    unsigned char b = (unsigned char)s[0];
    /* treat anything that is not control as printable for display (includes Cyrillic UTF-8 lead bytes) */
    if (b >= 0x20 && b != 0x7F) return 1;
    return 0;
}

int compare(const void *a, const void *b) {
    const ShannonCode *codeA = (const ShannonCode *)a;
    const ShannonCode *codeB = (const ShannonCode *)b;
    if (codeA->probability > codeB->probability) return -1;
    if (codeA->probability < codeB->probability) return 1;
    return 0;
}

int computeCodeLength(double probability) {
    if (probability <= 0) return 0;
    return (int)ceil(-log2(probability));
}

void toBinary(double number, int length, char *binary) {
    for (int i = 0; i < length; i++) {
        number *= 2;
        binary[i] = (number >= 1) ? '1' : '0';
        if (number >= 1) number -= 1;
    }
    binary[length] = '\0';
}

void buildShannonCodes(ShannonCode *codes, int count) {
    double cumulative = 0.0;
    
    for (int i = 0; i < count; i++) {
        codes[i].length = computeCodeLength(codes[i].probability);
        toBinary(cumulative, codes[i].length, codes[i].code);
        cumulative += codes[i].probability;
    }
}

int checkKraftInequality(ShannonCode *codes, int count) {
    double kraft_sum = 0.0;
    for (int i = 0; i < count; i++) {
        kraft_sum += pow(2, -codes[i].length);
    }
    return kraft_sum <= 1.0;
}

double computeKraftSum(ShannonCode *codes, int count) {
    double kraft_sum = 0.0;
    for (int i = 0; i < count; i++) {
        if (codes[i].length > 0) kraft_sum += pow(2.0, -codes[i].length);
    }
    return kraft_sum;
}

double computeEntropy(ShannonCode *codes, int count) {
    double entropy = 0.0;
    for (int i = 0; i < count; i++) {
        if (codes[i].probability > 0) {
            entropy -= codes[i].probability * log2(codes[i].probability);
        }
    }
    return entropy;
}

double computeAverageLength(ShannonCode *codes, int count) {
    double avg_length = 0.0;
    for (int i = 0; i < count; i++) {
        avg_length += codes[i].probability * codes[i].length;
    }
    return avg_length;
}

void encodeText(const char *text, ShannonCode *codes, int codeCount, char *encoded) {
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
        int k;
        for (k = 0; k < seqlen && text[i+k] != '\0' && k < MAX_UTF8_BYTES-1; ++k) sym[k] = text[i+k];
        sym[k] = '\0';

        for (int j = 0; j < codeCount; ++j) {
            if (strcmp(codes[j].symbol, sym) == 0) {
                strcat(encoded, codes[j].code);
                break;
            }
        }

        i += seqlen;
    }
}

TreeNode* buildTreeRecursive(ShannonCode *codes, int start, int end) {
    if (start > end) return NULL;
    
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    node->symbol[0] = '\0';
    node->left = NULL;
    node->right = NULL;
    
    if (start == end) {
        strcpy(node->symbol, codes[start].symbol);
        return node;
    }
    
    int mid = (start + end) / 2;
    node->left = buildTreeRecursive(codes, start, mid);
    node->right = buildTreeRecursive(codes, mid + 1, end);
    
    return node;
}

TreeNode* buildCodeTree(ShannonCode *codes, int count) {
    return buildTreeRecursive(codes, 0, count - 1);
}

void printTree(TreeNode *root, int level) {
    if (!root) return;
    
    for (int i = 0; i < level; i++) printf("  ");
    
    if (root->symbol[0] != '\0') {
        if (isPrintableUTF8(root->symbol)) {
            printf("'%s'\n", root->symbol);
        } else {
            /* print bytes in hex */
            printf("'");
            for (size_t bi = 0; bi < strlen(root->symbol); ++bi) printf("\\x%02X", (unsigned char)root->symbol[bi]);
            printf("'\n");
        }
    } else {
        printf("Node\n");
    }
    
    printTree(root->left, level + 1);
    printTree(root->right, level + 1);
}

void freeTree(TreeNode *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

double sumProbabilities(ShannonCode *codes, int count) {
    double s = 0.0;
    for (int i = 0; i < count; ++i) s += codes[i].probability;
    return s;
}

void writeGraphviz(ShannonCode *codes, int count, const char *dotFilename) {
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
                if (nodes[cur].left == -1) {
                    if (nodeCount >= MAX_NODES) break;
                    nodes[cur].left = nodeCount++;
                }
                cur = nodes[cur].left;
            } else {
                if (nodes[cur].right == -1) {
                    if (nodeCount >= MAX_NODES) break;
                    nodes[cur].right = nodeCount++;
                }
                cur = nodes[cur].right;
            }
        }

        if (codes[i].probability > 0.0) {
            const char *sym = codes[i].symbol;
            if (isPrintableUTF8(sym) && sym[0] != '\\' && sym[0] != '"') {
                snprintf(nodes[cur].label, sizeof(nodes[cur].label), "'%s'\\n%.6f", sym, codes[i].probability);
            } else {
                /* show hex bytes */
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
    fprintf(f, "digraph ShannonTree {\n");
    fprintf(f, "  node [shape=circle, fontname=\"Helvetica\"];\n");

    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].label[0] != '\0') {
            char esc[256];
            int k = 0;
            for (int p = 0; nodes[i].label[p] != '\0' && k < (int)sizeof(esc)-2; ++p) {
                if (nodes[i].label[p] == '"') esc[k++] = '\\', esc[k++] = '"';
                else esc[k++] = nodes[i].label[p];
            }
            esc[k] = '\0';
            fprintf(f, "  n%d [shape=box, label=\"%s\"];\n", i, esc);
        } else {
            fprintf(f, "  n%d [label=\"\"];\n", i);
        }
    }

    for (int i = 0; i < nodeCount; ++i) {
        if (nodes[i].left != -1) fprintf(f, "  n%d -> n%d [label=\"0\"];\n", i, nodes[i].left);
        if (nodes[i].right != -1) fprintf(f, "  n%d -> n%d [label=\"1\"];\n", i, nodes[i].right);
    }

    fprintf(f, "}\n");
    fclose(f);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o shannon_tree.png", dotFilename);

    system(cmd);
    free(nodes);
}

int main() {
    system("chcp 65001 > nul");
    const char *filename = "file.txt"; 

    /* Read UTF-8 symbols and their counts */
    char symbols[MAX_SYMBOLS][MAX_UTF8_BYTES];
    int counts[MAX_SYMBOLS];
    int totalSymbols = 0;
    int uniqueSymbols = 0;

    countFrequenciesUTF8(filename, symbols, counts, &uniqueSymbols, &totalSymbols);

    if (uniqueSymbols <= 0) {
        printf("Файл пуст или символы не найдены\n");
        return 0;
    }

    ShannonCode *codes = (ShannonCode*)malloc(uniqueSymbols * sizeof(ShannonCode));
    for (int i = 0; i < uniqueSymbols; ++i) {
        strncpy(codes[i].symbol, symbols[i], MAX_UTF8_BYTES-1);
        codes[i].symbol[MAX_UTF8_BYTES-1] = '\0';
        codes[i].probability = (double)counts[i] / totalSymbols;
    }


    if (uniqueSymbols > 0) {
        double p_sum = 0.0;
        for (int i = 0; i < uniqueSymbols; ++i) p_sum += codes[i].probability;
        if (p_sum > 0.0) {
            for (int i = 0; i < uniqueSymbols; ++i) codes[i].probability /= p_sum;
            double sum_after = 0.0;
            for (int i = 0; i < uniqueSymbols - 1; ++i) sum_after += codes[i].probability;
            codes[uniqueSymbols - 1].probability = 1.0 - sum_after;
        }
    }
    
    qsort(codes, uniqueSymbols, sizeof(ShannonCode), compare);
    
    buildShannonCodes(codes, uniqueSymbols);
    double p_sum = sumProbabilities(codes, uniqueSymbols);
    
    writeGraphviz(codes, uniqueSymbols, "shannon_tree.dot");
    
    printf("Код Шеннона\n");
    printf("Символ\tВероятность\t\tКодовое слово\t\tДлина\n");
    for (int i = 0; i < uniqueSymbols; i++) {
        /* Special-case common control characters to human-readable escapes */
        if (strlen(codes[i].symbol) == 1) {
            unsigned char b = (unsigned char)codes[i].symbol[0];
            if (b == '\n') {
                printf("'\\n'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length);
                continue;
            } else if (b == '\r') {
                printf("'\\r'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length);
                continue;
            } else if (b == '\t') {
                printf("'\\t'\t%10.6f\t%-20s\t%3d\n", codes[i].probability, codes[i].code, codes[i].length);
                continue;
            }
        }

        /* Print UTF-8 symbol string when possible, otherwise hex bytes */
        if (isPrintableUTF8(codes[i].symbol)) {
            printf("'%s'\t%10.6f\t%-20s\t%3d\n",
                   codes[i].symbol, codes[i].probability, codes[i].code, codes[i].length);
        } else {
            /* print bytes as hex */
            for (size_t bi = 0; bi < strlen(codes[i].symbol); ++bi) {
                if (bi) printf(" ");
                printf("0x%02X", (unsigned char)codes[i].symbol[bi]);
            }
            printf("\t%10.6f\t%-20s\t%3d\n",
                   codes[i].probability, codes[i].code, codes[i].length);
        }
    }
    printf("\n");
    printf("Сумма вероятностей: %.15f\n", p_sum);
    
    double kraft_sum = computeKraftSum(codes, uniqueSymbols);
    double entropy = computeEntropy(codes, uniqueSymbols);
    double avg_length = computeAverageLength(codes, uniqueSymbols);
    double redundancy = avg_length - entropy;

    printf("Код Шеннона\n");
    /* Print Kraft equation: sum 2^{-l_i} = value */
    printf("Неравенство Крафта: ");
    int printed = 0;
    for (int i = 0; i < uniqueSymbols; ++i) {
        if (codes[i].length <= 0) continue;
        if (printed) printf(" + ");
        printf("2^-%d", codes[i].length);
        printed++;
    }
    if (!printed) printf("0");
    printf(" = %.15f\n", kraft_sum);
    printf("%s\n", (kraft_sum <= 1.0) ? "Выполнено" : "Не выполнено");
    printf("Энтропия исходного текста: %.6f\n", entropy);
    printf("Средняя длина кодового слова: %.6f\n", avg_length);
    printf("Избыточность кода Шеннона: %.6f\n\n", redundancy);
    
    char sample_text[101];
    FILE *file = fopen(filename, "rb");
    if (file) {
        fread(sample_text, 1, 100, file);
        sample_text[100] = '\0';
        fclose(file);
    } else {
        strcpy(sample_text, "Пример текста для кодирования");
    }
    
    char encoded[100 * MAX_CODE_LENGTH] = "";
    encodeText(sample_text, codes, uniqueSymbols, encoded);
    
    int original_bits = 100 * 8;
    int encoded_bits = strlen(encoded);
    double compression_ratio = (double)original_bits / encoded_bits;
    
    printf("Исходный текст (100 символов): %s\n", sample_text);
    printf("Закодированная последовательность: %s\n", encoded);
    printf("Длина закодированной последовательности: %d бит\n", encoded_bits);
    printf("Коэффициент сжатия: %.6f\n\n", compression_ratio);
    
    printf("Кодовое дерево Шеннона:\n");
    TreeNode *root = buildCodeTree(codes, uniqueSymbols);
    freeTree(root);
    
    free(codes);
    return 0;
}