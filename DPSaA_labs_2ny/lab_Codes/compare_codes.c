/* compare_codes.c
 * Compare Shannon, Fano, Huffman, Gilbert–Moore codes
 * Produces table of: Kraft inequality, Entropy, Average length, Redundancy
 */
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
    double Q; /* used by Gilbert-Moore */
} CodeEntry;

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
    (void)read; fclose(file);
    for (int i = 0; i < MAX_SYMBOLS; ++i) counts[i] = 0;
    *unique = 0; *total = 0;
    long i = 0;
    while (i < filelen) {
        unsigned char b = buf[i]; int seqlen = 1;
        if ((b & 0x80) == 0x00) seqlen = 1;
        else if ((b & 0xE0) == 0xC0) seqlen = 2;
        else if ((b & 0xF0) == 0xE0) seqlen = 3;
        else if ((b & 0xF8) == 0xF0) seqlen = 4;
        else seqlen = 1;
        if (i + seqlen > filelen) seqlen = (int)(filelen - i);
        char sym[MAX_UTF8_BYTES]; int k;
        for (k = 0; k < seqlen && k < MAX_UTF8_BYTES-1; ++k) sym[k] = (char)buf[i+k]; sym[k] = '\0';
        int found = -1;
        for (int s = 0; s < *unique; ++s) if (strcmp(symbols[s], sym) == 0) { found = s; break; }
        if (found == -1) { if (*unique >= MAX_SYMBOLS) break; strcpy(symbols[*unique], sym); counts[*unique] = 1; (*unique)++; }
        else counts[found]++;
        (*total)++; i += seqlen;
    }
    free(buf);
}

double computeEntropy(CodeEntry *codes, int count) {
    double entropy = 0.0; for (int i = 0; i < count; ++i) if (codes[i].probability > 0) entropy -= codes[i].probability * log2(codes[i].probability); return entropy;
}
double computeAverageLength(CodeEntry *codes, int count) { double avg = 0.0; for (int i = 0; i < count; ++i) avg += codes[i].probability * codes[i].length; return avg; }
double computeKraftSum(CodeEntry *codes, int count) { double sum = 0.0; for (int i = 0; i < count; ++i) if (codes[i].length > 0) sum += pow(2.0, -codes[i].length); return sum; }

/* Binary fractional conversion helpers */
void toBinary(double number, int length, char *binary) { for (int i = 0; i < length; ++i) { number *= 2; binary[i] = (number >= 1) ? '1' : '0'; if (number >= 1) number -= 1; } binary[length] = '\0'; }
void fracToBinary(double frac, int len, char *out) { for (int i = 0; i < len; ++i) { frac *= 2.0; if (frac >= 1.0) { out[i] = '1'; frac -= 1.0; } else out[i] = '0'; } out[len] = '\0'; }

/* ---------- SHANNON ---------- */
int compareProbDesc(const void *a, const void *b) { const CodeEntry *A = (const CodeEntry*)a, *B = (const CodeEntry*)b; if (A->probability > B->probability) return -1; if (A->probability < B->probability) return 1; return 0; }
int computeCodeLength_shannon(double probability) { if (probability <= 0) return 0; return (int)ceil(-log2(probability)); }
void buildShannonCodes(CodeEntry *orig, int count, CodeEntry *out) {
    /* Copy & sort by prob desc, compute length & code, then map back by symbol to out */
    CodeEntry *tmp = (CodeEntry*)malloc(count * sizeof(CodeEntry)); for (int i = 0; i < count; ++i) tmp[i] = orig[i]; qsort(tmp, count, sizeof(CodeEntry), compareProbDesc);
    double cumulative = 0.0;
    for (int i = 0; i < count; ++i) { int li = computeCodeLength_shannon(tmp[i].probability); tmp[i].length = li; toBinary(cumulative, li, tmp[i].code); cumulative += tmp[i].probability; }
    /* map back */
    for (int i = 0; i < count; ++i) { out[i].length = 0; out[i].code[0] = '\0'; }
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < count; ++j) if (strcmp(tmp[i].symbol, orig[j].symbol) == 0) { strcpy(out[j].code, tmp[i].code); out[j].length = tmp[i].length; break; }
    }
    free(tmp);
}

/* ---------- FANO ---------- */
 /* find median index to split range L..R */
int findMedianIndex(CodeEntry *codes, int L, int R) {
    if (L >= R) return L; double total = 0.0; for (int i = L; i <= R; ++i) total += codes[i].probability; double acc = 0.0; int best = L; double bestDiff = 1e300; for (int i = L; i < R; ++i) { acc += codes[i].probability; double right = total - acc; double diff = fabs(acc - right); if (diff < bestDiff) { bestDiff = diff; best = i; } } return best;
}
void appendBitFano(CodeEntry *codes, int L, int R, char bit) { for (int i = L; i <= R; ++i) { size_t len = strlen(codes[i].code); if (len + 1 < MAX_CODE_LENGTH) { codes[i].code[len] = bit; codes[i].code[len+1] = '\0'; codes[i].length = (int)strlen(codes[i].code); } } }
void fanoRecursive(CodeEntry *codes, int L, int R) { if (L >= R) return; int m = findMedianIndex(codes, L, R); appendBitFano(codes, L, m, '0'); appendBitFano(codes, m+1, R, '1'); fanoRecursive(codes, L, m); fanoRecursive(codes, m+1, R); }
void buildFanoCodes(CodeEntry *orig, int count, CodeEntry *out) {
    CodeEntry *tmp = (CodeEntry*)malloc(count * sizeof(CodeEntry)); for (int i = 0; i < count; ++i) tmp[i] = orig[i]; qsort(tmp, count, sizeof(CodeEntry), compareProbDesc);
    for (int i = 0; i < count; ++i) { tmp[i].code[0] = '\0'; tmp[i].length = 0; }
    fanoRecursive(tmp, 0, count-1);
    for (int i = 0; i < count; ++i) { out[i].length = 0; out[i].code[0] = '\0'; }
    for (int i = 0; i < count; ++i) { for (int j = 0; j < count; ++j) if (strcmp(tmp[i].symbol, orig[j].symbol) == 0) { strcpy(out[j].code, tmp[i].code); out[j].length = tmp[i].length; break; } }
    free(tmp);
}

/* ---------- HUFFMAN ---------- */
typedef struct HuffNode { char symbol[MAX_UTF8_BYTES]; double probability; struct HuffNode *left, *right; int is_leaf; } HuffNode;
HuffNode* buildHuffmanTree(HuffNode **nodes, int *n) {
    if (*n == 0) return NULL; while (*n > 1) { int i_min = -1, j_min = -1; for (int i = 0; i < *n; ++i) { if (i_min == -1 || nodes[i]->probability < nodes[i_min]->probability) i_min = i; } for (int j = 0; j < *n; ++j) { if (j == i_min) continue; if (j_min == -1 || nodes[j]->probability < nodes[j_min]->probability) j_min = j; } int a = i_min, b = j_min; if (a > b) { int t=a; a=b; b=t; } HuffNode *newNode = (HuffNode*)malloc(sizeof(HuffNode)); newNode->symbol[0] = '\0'; newNode->left = nodes[a]; newNode->right = nodes[b]; newNode->is_leaf = 0; newNode->probability = nodes[a]->probability + nodes[b]->probability; for (int k = b+1; k < *n; ++k) nodes[k-1] = nodes[k]; for (int k = a+1; k < *n-1; ++k) nodes[k-1] = nodes[k]; nodes[*n-2] = newNode; *n = *n - 1; } return nodes[0]; }
void generateCodesFromTree(HuffNode *root, CodeEntry *orig, int count, char *prefix, int depth) {
    if (!root) return; if (root->is_leaf) { for (int i = 0; i < count; ++i) if (strcmp(orig[i].symbol, root->symbol) == 0) { strncpy(orig[i].code, prefix, MAX_CODE_LENGTH-1); orig[i].code[MAX_CODE_LENGTH-1] = '\0'; orig[i].length = depth; break; } return; }
    if (root->left) { if (depth < MAX_CODE_LENGTH-1) { prefix[depth] = '0'; prefix[depth+1] = '\0'; generateCodesFromTree(root->left, orig, count, prefix, depth+1); } }
    if (root->right) { if (depth < MAX_CODE_LENGTH-1) { prefix[depth] = '1'; prefix[depth+1] = '\0'; generateCodesFromTree(root->right, orig, count, prefix, depth+1); } }
}
void freeTree(HuffNode *n) { if (!n) return; freeTree(n->left); freeTree(n->right); free(n); }

void buildHuffmanCodes(CodeEntry *orig, int count, CodeEntry *out) {
    HuffNode *nodesList[MAX_SYMBOLS]; int nNodes = 0; for (int i = 0; i < count; ++i) { HuffNode *nd = (HuffNode*)malloc(sizeof(HuffNode)); nd->left = nd->right = NULL; nd->is_leaf = 1; strncpy(nd->symbol, orig[i].symbol, MAX_UTF8_BYTES-1); nd->symbol[MAX_UTF8_BYTES-1] = '\0'; nd->probability = orig[i].probability; nodesList[nNodes++] = nd; }
    HuffNode *root = buildHuffmanTree(nodesList, &nNodes);
    for (int i = 0; i < count; ++i) { out[i].length = 0; out[i].code[0] = '\0'; }
    char prefix[MAX_CODE_LENGTH]; prefix[0] = '\0'; generateCodesFromTree(root, out, count, prefix, 0);
    freeTree(root);
}

/* ---------- GILBERT-MOORE ---------- */
int compareSymbolLex(const void *a, const void *b) { const CodeEntry *A = (const CodeEntry*)a; const CodeEntry *B = (const CodeEntry*)b; return strcmp(A->symbol, B->symbol); }
void buildGilbertMooreCodes(CodeEntry *orig, int count, CodeEntry *out) {
    CodeEntry *order = (CodeEntry*)malloc(count * sizeof(CodeEntry)); for (int i = 0; i < count; ++i) order[i] = orig[i]; qsort(order, count, sizeof(CodeEntry), compareSymbolLex);
    double cumulative = 0.0; for (int i = 0; i < count; ++i) { order[i].Q = cumulative + order[i].probability / 2.0; cumulative += order[i].probability; }
    for (int i = 0; i < count; ++i) {
        if (order[i].probability <= 0) { order[i].length = 0; order[i].code[0] = '\0'; continue; }
        int li = (int)ceil(-log2(order[i].probability)) + 1; if (li < 1) li = 1; if (li > MAX_CODE_LENGTH-1) li = MAX_CODE_LENGTH-1; order[i].length = li; double q = order[i].Q - floor(order[i].Q); fracToBinary(q, li, order[i].code);
    }
    /* map back */ for (int i = 0; i < count; ++i) { out[i].length = 0; out[i].code[0] = '\0'; }
    for (int i = 0; i < count; ++i) for (int j = 0; j < count; ++j) if (strcmp(order[i].symbol, orig[j].symbol) == 0) { strcpy(out[j].code, order[i].code); out[j].length = order[i].length; out[j].Q = order[i].Q; break; }
    free(order);
}

/* ---------- Table & main ---------- */
void copySymbolsToEntries(char symbols[][MAX_UTF8_BYTES], int *counts, int unique, int total, CodeEntry *entries) {
    for (int i = 0; i < unique; ++i) {
        strncpy(entries[i].symbol, symbols[i], MAX_UTF8_BYTES-1);
        entries[i].symbol[MAX_UTF8_BYTES-1] = '\0'; entries[i].probability = (double)counts[i] / total; entries[i].code[0] = '\0'; entries[i].length = 0; entries[i].Q = 0.0;
    }
}

int main(int argc, char **argv) {
    system("chcp 65001 > nul");
    const char *filename = (argc > 1) ? argv[1] : "file.txt";
    char symbols[MAX_SYMBOLS][MAX_UTF8_BYTES]; int counts[MAX_SYMBOLS]; int unique = 0, total = 0;
    countFrequenciesUTF8(filename, symbols, counts, &unique, &total);
    if (unique <= 0) { printf("Файл пуст или символы не найдены\n"); return 0; }
    CodeEntry *base = (CodeEntry*)malloc(unique * sizeof(CodeEntry)); copySymbolsToEntries(symbols, counts, unique, total, base);

    CodeEntry *sh = (CodeEntry*)malloc(unique * sizeof(CodeEntry)); memcpy(sh, base, unique * sizeof(CodeEntry)); buildShannonCodes(base, unique, sh);
    CodeEntry *fa = (CodeEntry*)malloc(unique * sizeof(CodeEntry)); memcpy(fa, base, unique * sizeof(CodeEntry)); buildFanoCodes(base, unique, fa);
    CodeEntry *hu = (CodeEntry*)malloc(unique * sizeof(CodeEntry)); memcpy(hu, base, unique * sizeof(CodeEntry)); buildHuffmanCodes(base, unique, hu);
    CodeEntry *gm = (CodeEntry*)malloc(unique * sizeof(CodeEntry)); memcpy(gm, base, unique * sizeof(CodeEntry)); buildGilbertMooreCodes(base, unique, gm);

    double sh_entropy = computeEntropy(sh, unique); double sh_avg = computeAverageLength(sh, unique); double sh_kraft = computeKraftSum(sh, unique); double sh_red = sh_avg - sh_entropy;
    double fa_entropy = computeEntropy(fa, unique); double fa_avg = computeAverageLength(fa, unique); double fa_kraft = computeKraftSum(fa, unique); double fa_red = fa_avg - fa_entropy;
    double hu_entropy = computeEntropy(hu, unique); double hu_avg = computeAverageLength(hu, unique); double hu_kraft = computeKraftSum(hu, unique); double hu_red = hu_avg - hu_entropy;
    double gm_entropy = computeEntropy(gm, unique); double gm_avg = computeAverageLength(gm, unique); double gm_kraft = computeKraftSum(gm, unique); double gm_red = gm_avg - gm_entropy;

    printf("\nСравнение кодов для файла '%s' (символов %d, байт %d):\n\n", filename, unique, total);
    /* Use ASCII table headers with fixed widths so printf alignment remains stable in console */
    const int COL_ALG = 18, COL_KRAFT = 22, COL_ENT = 12, COL_AVG = 14, COL_RED = 12;
    char headerFmt[128]; char sepFmt[128]; char rowFmt[128];
    snprintf(headerFmt, sizeof(headerFmt), "%%-%ds | %%-%ds | %%-%ds | %%-%ds | %%-%ds\n", COL_ALG, COL_KRAFT, COL_ENT, COL_AVG, COL_RED);
    snprintf(sepFmt, sizeof(sepFmt), "%%-%ds-+-%%-%ds-+-%%-%ds-+-%%-%ds-+-%%-%ds\n", COL_ALG, COL_KRAFT, COL_ENT, COL_AVG, COL_RED);
    snprintf(rowFmt, sizeof(rowFmt), "%%-%ds | %%%d.15f | %%%d.6f | %%%d.6f | %%%d.6f\n", COL_ALG, COL_KRAFT, COL_ENT-2, COL_AVG-2, COL_RED-2);
    printf(headerFmt, "Algorithm", "Kraft sum (sum 2^-l)", "Entropy", "Avg length", "Redundancy");
    /* print separator with '-' characters */
    for (int i = 0; i < COL_ALG; ++i) putchar('-'); printf("-+-"); for (int i = 0; i < COL_KRAFT; ++i) putchar('-'); printf("-+-"); for (int i = 0; i < COL_ENT; ++i) putchar('-'); printf("-+-"); for (int i = 0; i < COL_AVG; ++i) putchar('-'); printf("-+-"); for (int i = 0; i < COL_RED; ++i) putchar('-'); putchar('\n');
    printf(rowFmt, "Shannon", sh_kraft, sh_entropy, sh_avg, sh_red);
    printf(rowFmt, "Fano", fa_kraft, fa_entropy, fa_avg, fa_red);
    printf(rowFmt, "Huffman", hu_kraft, hu_entropy, hu_avg, hu_red);
    printf(rowFmt, "Gilbert-Moore", gm_kraft, gm_entropy, gm_avg, gm_red);
free(base); free(sh); free(fa); free(hu); free(gm);
    return 0;
}
