#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int brute_force_comparisons = 0;
int rabin_karp_comparisons = 0;

void rabin_karp_search(const char* text, const char* pattern, int* results, int* result_count, long q);
void brute_force_search(const char* text, const char* pattern, int* results, int* result_count);

void func_eff() {
    const char* random_text ="slkdfjksdfhksjdfuskubas,jk.aweilwdklnresncsnnufelqkhdxpdvplgqzskldjhskdfhdksfhresncsnnufelqkhdxpdvplgqzkjsdfkjshdfukhsfkjshfasldkjaslkdjalsdjlakdjresncsnnufelqkhdxpdvplgqzkjslksdjflksdjflkdsjfiwfhkjsdbfliwauwafaweu uwahfkJBVliwegfwesgbejnfwhefwne resncsnnufelqkhdxpdvplgqzwehfwufhwek,jfweh uiuedk,wefhwuefhwelfhlkefdnjksegwekfhwkukhresncsnnufelqkhdxpdvplgqzresncsnnufelqkhdxpdvplgqzresncsnnufelqkhdxpdvplgqzkue";

        
    const char* base_pattern = "resncsnnufelqkhdxpdvplgqz";
    long q = 1000000007;

    
    int max_results = 1000;
    int rabin_karp_results[max_results];
    int brute_force_results[max_results];
    int rabin_karp_count;
    int brute_force_count;

    printf("========================================================================\n");
    printf("Длина подстроки | Кол-во сравнений Рабина-Карпа | Кол-во сравнений прямым поиском\n");
    printf("------------------------------------------------------------------------\n");

    for (int len = 1; len <= (int)strlen(base_pattern); len++) {
        char pattern[256];
        strncpy(pattern, base_pattern, len);
        pattern[len] = '\0';

        rabin_karp_comparisons = 0;
        brute_force_comparisons = 0;

        rabin_karp_search(random_text, pattern, rabin_karp_results, &rabin_karp_count, q);
        brute_force_search(random_text, pattern, brute_force_results, &brute_force_count);

        printf("%15d | %30d | %30d\n", len, rabin_karp_comparisons, brute_force_comparisons);
    }
}

void brute_force_search(const char* text, const char* pattern, int* results, int* result_count) {
    int n = strlen(text);
    int m = strlen(pattern);
    *result_count = 0;
    brute_force_comparisons = 0;

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            brute_force_comparisons++;
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        if (j == m) {
            results[(*result_count)++] = i;
        }
    }
}

long calculate_hash(const char* str, int start, int end, long q) {
    long hash = 0;
    for (int i = start; i <= end; i++) {
        hash = (hash * 256 + (unsigned char)str[i]) % q;
    }
    return hash;
}

void rabin_karp_search(const char* text, const char* pattern, int* results, int* result_count, long q) {
    int n = strlen(text);
    int m = strlen(pattern);
    *result_count = 0;
    rabin_karp_comparisons = 0;

    if (n < m || m == 0) {
        return;
    }

    long pattern_hash = calculate_hash(pattern, 0, m - 1, q);
    long text_hash = calculate_hash(text, 0, m - 1, q);

    long coeff = 1;
    for (int i = 0; i < m - 1; i++) {
        coeff = (coeff * 256) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (text_hash == pattern_hash) {
            int j;
            for (j = 0; j < m; j++) {
                rabin_karp_comparisons++;
                if (text[i + j] != pattern[j]) {
                    break;
                }
            }
            if (j == m) {
                results[(*result_count)++] = i;
            }
        }

        if (i < n - m) {
            text_hash = (256 * (text_hash - (unsigned char)text[i] * coeff) + (unsigned char)text[i + m]) % q;
            if (text_hash < 0) {
                text_hash += q;
            }
        }
    }
}

int main() {
    const char* text = "According to all known laws of aviation, there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee, of course, flies anyway because bees don't care what humans think is impossible. Yellow, black. Yellow, black. Yellow, black. Yellow, black. Ooh, black and yellow! Let's shake it up a little. Barry! Breakfast is ready! Coming! Hang on a second. Hello? Barry? Adam? Can you believe this is happening? I can't. I'll pick you up. Looking sharp. Use the stairs, Your father paid good money for those. Sorry. I'm excited. Here's the graduate. We're very proud of you, son. A perfect report card, all B's. Very proud. Ma! I got a thing going here. You got lint on your fuzz. Ow! That's me! Wave to us! We'll be in row 118,000. Bye! Barry, I told you, stop flying in the house! Hey, Adam. Hey, Barry. Is that fuzz gel? A little. Special day, graduation. Never thought I'd make it. Three days grade school, three days high school. Those were awkward. Three days college. I'm glad I took a day and hitchhiked around The Hive. You did come back different. Hi, Barry. Artie, growing a mustache? Looks good. Hear about Frankie? Yeah. You going to the funeral? No, I'm not going. Everybody knows, sting someone, you die. Don't waste it on a squirrel. Such a hothead. I guess he could have just gotten out of the way. I love this incorporating an amusement park into our day. That's why we don't need vacations. Boy, quite a bit of pomp under the circumstances. Well, Adam, today we are men. We are! Bee-men. Amen! Hallelujah! Students, faculty, distinguished bees, please welcome Dean Buzzwell. Welcome, New Hive City graduating class of 9:15. That concludes our ceremonies And begins your career at Honex Industries! Will we pick our job today? I heard it's just orientation. Heads up! Here we go. Keep your hands and antennas inside the tram at all times. Wonder what it'll be like? A little scary. Welcome to Honex, a division of Honesco and a part of the Hexagon Group. This is it! Wow. Wow. We know that you, as a bee, have worked your whole life to get to the point where you can work for your whole life. Honey begins when our valiant Pollen Jocks bring the nectar to The Hive. Our top-secret formula is automatically color-corrected, scent-adjusted and bubble-contoured into this soothing sweet syrup with its distinctive golden glow you know as... Honey! That girl was hot. She's my cousin! She is? Yes, we're all cousins. Right. You're right. At Honex, we constantly strive to improve every aspect of bee existence. These bees are stress-testing a new helmet technology. What do you think he makes? Not enough. Here we have our latest advancement, the Krelman. What does that do? Catches that little strand of honey that hangs after you pour it. Saves us millions. Can anyone work on the Krelman? Of course. Most bee jobs are small ones. But bees know that every small job, if it's done well, means a lot. But choose carefully because you'll stay in the job you pick for the rest of your life. The same job the rest of your life? I didn't know that. What's the difference? You'll be happy to know that bees, as a species, haven't had one day off in 27 million years. So you'll just work us to death? We'll sure try. Wow! That blew my mind! 'What's the difference?' How can you say that? One job forever? That's an insane choice to have to make. I'm relieved. Now we only have to make one decision in life. But, Adam, how could they never have told us that? Why would you question anything? We're bees. We're the most perfectly functioning society on Earth. You ever think maybe things work a little too well here? Like what? Give me one example. I don't know. But you know what I'm talking about. Please clear the gate. Royal Nectar Force on approach. Wait a second. Check it out. Hey, those are Pollen Jocks! Wow. I've never seen them this close. They know what it's like outside The Hive. Yeah, but some don't come back. Hey, Jocks! Hi, Jocks! You guys did great! You're monsters! You're sky freaks! I love it! I love it! I wonder where they were. I don't know. Their day's not planned. Outside The Hive, flying who knows where, doing who knows what. You can't just decide to be a Pollen Jock. You have to be bred for that. Right. Look. That's more pollen than you and I will see in a lifetime. It's just a status symbol. Bees make too much of it. Perhaps. Unless you're wearing it and the ladies see you wearing it. Those ladies? Aren't they our cousins too? Distant. Distant. Look at these two. Couple of Hive Harrys. Let's have fun with them. It must be dangerous being a Pollen Jock. Yeah. Once a bear pinned me against a mushroom! He had a paw on my throat, and with the other, he was slapping me! Oh, my! I never thought I'd knock him out. What were you doing during this? Trying to alert the authorities. I can autograph that.";
    //const char* pattern = "bee";
    printf("Текст: %s\n", text);
    int q=101;

    printf("Введите слово для поиска: ");
    char pattern[50];
    scanf("%s", pattern);

    int max_results = 100;
    int brute_force_results[max_results];
    int rabin_karp_results[max_results];
    int brute_force_count, rabin_karp_count;

    printf("Подстрока для поиска: %s\n\n", pattern);

    printf("=== Метод прямого поиска ===\n");
    brute_force_search(text, pattern, brute_force_results, &brute_force_count);
    
    if (brute_force_count > 0) {
        printf("Найдено вхождений: %d\n", brute_force_count);
        printf("Индексы начала: ");
        for (int i = 0; i < brute_force_count; i++) {
            printf("%d ", brute_force_results[i]);
        }
        printf("\n");
    } else {
        printf("Вхождений не найдено\n");
    }

    printf("\n=== Метод Рабина-Карпа ===\n");
    rabin_karp_search(text, pattern, rabin_karp_results, &rabin_karp_count, q);
    
    if (rabin_karp_count > 0) {
        printf("Найдено вхождений: %d\n", rabin_karp_count);
        printf("Индексы начала: ");
        for (int i = 0; i < rabin_karp_count; i++) {
            printf("%d ", rabin_karp_results[i]);
        }
        printf("\n");
    } else {
        printf("Вхождений не найдено\n");
    }

    printf("\n=== Сравнение количества посимвольных сравнений ===\n");
    printf("Метод перебора: %d сравнений\n", brute_force_comparisons);
    printf("Метод Рабина-Карпа: %d сравнений\n", rabin_karp_comparisons);
    
    if (brute_force_comparisons > 0) {
        double ratio = (double)rabin_karp_comparisons / brute_force_comparisons * 100;
        printf("Эффективность: метод Рабина-Карпа использует %.2f%% от сравнений метода перебора\n", ratio);
    }

    func_eff();

    // Запуск перебора (один раз)
    brute_force_search(text, pattern, brute_force_results, &brute_force_count);

    // Таблица сравнения
    long q_values[] = {101, 1009, 10007, 1000000007};
    int q_count = sizeof(q_values) / sizeof(q_values[0]);

    printf("====================================================================================\n");
    printf("| %-12s | %-20s | %-20s | %-20s |\n", "q", "Перебор (сравн.)", "Рабин-Карп (сравн.)", "Эффективность (%)");
    printf("====================================================================================\n");

    for (int k = 0; k < q_count; k++) {
        long q = q_values[k];
        rabin_karp_search(text, pattern, rabin_karp_results, &rabin_karp_count, q);

        double ratio = brute_force_comparisons > 0 
            ? (double)rabin_karp_comparisons / brute_force_comparisons * 100 
            : 0.0;

        printf("| %-12ld | %-20d | %-20d | %-20.2f |\n", 
               q, brute_force_comparisons, rabin_karp_comparisons, ratio);
    }

    printf("====================================================================================\n");

    return 0;
}
