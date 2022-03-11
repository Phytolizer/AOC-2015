#include <advent.h>
#include <advent/pcre.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char* input = advent_readEntireFile(ADVENT_INPUT);
    if (input == NULL) {
        return 1;
    }

    int errCode;
    size_t errOfs;
    pcre2_code* linePattern = pcre2_compile(
        (PCRE2_SPTR) "^(\\d+)x(\\d+)x(\\d+)$", PCRE2_ZERO_TERMINATED,
        PCRE2_MULTILINE, &errCode, &errOfs, NULL);
    if (linePattern == NULL) {
        char errMsg[120];
        pcre2_get_error_message(errCode, (PCRE2_UCHAR*)errMsg, 120);
        fprintf(stderr, "Failed to compile regex: %s\n", errMsg);
    }

    size_t offset = 0;
    pcre2_match_data* matchData =
        pcre2_match_data_create_from_pattern(linePattern, NULL);
    long result = 0;
    long result2 = 0;

    while (true) {
        int rc = pcre2_match(linePattern, (PCRE2_SPTR)input,
                             PCRE2_ZERO_TERMINATED, offset, 0, matchData, NULL);
        if (rc == PCRE2_ERROR_NOMATCH) {
            break;
        } else if (rc < 0) {
            char errMsg[120];
            pcre2_get_error_message(rc, (PCRE2_UCHAR*)errMsg, 120);
            fprintf(stderr, "Regex match failed: %s\n", errMsg);
            pcre2_match_data_free(matchData);
            pcre2_code_free(linePattern);
            free(input);
            return 1;
        }

        size_t* ovector = pcre2_get_ovector_pointer(matchData);
        long l = strtol(&input[ovector[2]], NULL, 10);
        long w = strtol(&input[ovector[4]], NULL, 10);
        long h = strtol(&input[ovector[6]], NULL, 10);

        long sides[] = {
            l * w,
            w * h,
            h * l,
        };
        long smallestSide =
            advent_minElement(sides, sides + sizeof(sides) / sizeof(long));
        long surfaceArea = 2 * (sides[0] + sides[1] + sides[2]) + smallestSide;
        result += surfaceArea;

        long edges[] = {
            l,
            w,
            h,
        };
        long* ribbonLengths = malloc(sizeof(long));
        size_t ribbonLengthsCount = 0;
        size_t ribbonLengthsCap = 1;
        for (int i = 0; i < sizeof(edges) / sizeof(long); ++i) {
            for (int j = 0; j < sizeof(edges) / sizeof(long); ++j) {
                if (i == j) {
                    continue;
                }
                if (ribbonLengthsCount == ribbonLengthsCap) {
                    ribbonLengthsCap *= 2;
                    long* newPtr =
                        realloc(ribbonLengths, ribbonLengthsCap * sizeof(long));
                    if (newPtr == NULL) {
                        fprintf(stderr, "Fatal realloc error: returned NULL\n");
                        return 1;
                    }
                    ribbonLengths = newPtr;
                }
                ribbonLengths[ribbonLengthsCount] = 2 * edges[i] + 2 * edges[j];
                ++ribbonLengthsCount;
            }
        }
        result2 += advent_minElement(ribbonLengths,
                                     ribbonLengths + ribbonLengthsCount) +
                   l * w * h;
        free(ribbonLengths);

        offset = ovector[1];
    }

    printf("Part 1: %ld\n", result);
    printf("Part 2: %ld\n", result2);

    pcre2_match_data_free(matchData);
    pcre2_code_free(linePattern);
    free(input);
    return 0;
}
