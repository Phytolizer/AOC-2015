#include <advent.h>
#include <malloc.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <string.h>
#include <tgmath.h>

static const char HEX[] = "0123456789ABCDEF";

static void itoa(char* string, size_t n, size_t buflen);

#define ilog2(n) (64 - __builtin_clzl((uint64_t)n))
static uint64_t ipow10[] = {1,   1e1, 1e2, 1e3,  1e4,  1e5, 1e6,
                            1e7, 1e8, 1e9, 1e10, 1e11, 1e12};

static int16_t ilog10(uint64_t x) {
    if (x == 0) {
        // this is obviously not true, but it is required to make cmp_func work.
        return 0;
    }
    static int16_t log10_estimates[] = {0, 0, 1, 1, 1, 2, 2, 2, 3, 3,  3,
                                        4, 4, 4, 4, 5, 5, 5, 6, 6, 6,  7,
                                        7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10};

    int16_t estimated_log10 = log10_estimates[ilog2(x)];
    return estimated_log10 + (int16_t)(x >= ipow10[estimated_log10]);
}

int main(void) {
    uint8_t digest[MD5_DIGEST_LENGTH];
    char* input = advent_readEntireFile(ADVENT_INPUT);
    size_t inputLen = strlen(input);
    bool part1 = false;
    for (size_t i = 1; i < 10000000; ++i) {
        size_t keyLen = inputLen + ilog10(i);
        char* key = malloc(keyLen + 1);
        memcpy(key, input,
               inputLen); // NOLINT(bugprone-not-null-terminated-result)
        itoa(&key[inputLen], i, keyLen - inputLen);
        key[keyLen] = '\0';

        MD5((const uint8_t*)key, keyLen, digest);
        char stringDigest[MD5_DIGEST_LENGTH * 2 + 1];
        for (size_t j = 0; j < MD5_DIGEST_LENGTH; ++j) {
            stringDigest[j * 2] = HEX[digest[j] >> 4];
            stringDigest[j * 2 + 1] = HEX[digest[j] & 0x0F];
        }
        if (!part1 && memcmp(stringDigest, "00000", 5) == 0) {
            printf("Part 1: %zu\n", i);
            part1 = true;
        }
        if (memcmp(stringDigest, "000000", 6) == 0) {
            printf("Part 2: %zu\n", i);
            free(key);
            break;
        }

        free(key);
    }

    free(input);
}

static void itoa(char* string, size_t n, size_t buflen) {
    static const char digits[] = "0123456789";
    char* p = string + buflen;
    do {
        --p;
        *p = digits[n % 10];
        n /= 10;
    } while (n != 0);
}
