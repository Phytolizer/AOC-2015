#include <advent.h>
#include <advent/pcre.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    ACTION_TURN_ON,
    ACTION_TURN_OFF,
    ACTION_TOGGLE,
} Action;

int main(void) {
    int errCode;
    size_t errOfs;
    pcre2_code* linePattern = pcre2_compile(
        (PCRE2_SPTR) "(?<action>turn on|turn off|toggle) "
                     "(?<tlx>\\d+),(?<tly>\\d+) through "
                     "(?<brx>\\d+),(?<bry>\\d+)",
        PCRE2_ZERO_TERMINATED, PCRE2_MULTILINE, &errCode, &errOfs, NULL);
    if (linePattern == NULL) {
        char message[120];
        pcre2_get_error_message(errCode, (PCRE2_UCHAR*)message, 120);
        fprintf(stderr, "PCRE compile error: %s\n", message);
        return 1;
    }

    bool** grid = malloc(1000 * sizeof(bool*));
    size_t** grid2 = malloc(1000 * sizeof(size_t*));
    for (size_t i = 0; i < 1000; ++i) {
        grid[i] = calloc(1000, sizeof(bool));
        grid2[i] = calloc(1000, sizeof(size_t));
    }

    FILE* fp = fopen(ADVENT_INPUT, "r");
    char* line = NULL;
    size_t lineLen = 0;
    pcre2_match_data* matchData =
        pcre2_match_data_create_from_pattern(linePattern, NULL);
    while (getline(&line, &lineLen, fp) != -1) {
        int rc = pcre2_match(linePattern, (PCRE2_SPTR)line,
                             PCRE2_ZERO_TERMINATED, 0, 0, matchData, NULL);
        if (rc == PCRE2_ERROR_NOMATCH) {
            continue;
        }
        if (rc < 0) {
            char message[120];
            pcre2_get_error_message(errCode, (PCRE2_UCHAR*)message, 120);
            fprintf(stderr, "PCRE match error: %s\n", message);
            return 1;
        }

        size_t* ovector = pcre2_get_ovector_pointer(matchData);
        Action action;
        if (strncmp(&line[ovector[2]], "turn on", 7) == 0) {
            action = ACTION_TURN_ON;
        } else if (strncmp(&line[ovector[2]], "turn off", 8) == 0) {
            action = ACTION_TURN_OFF;
        } else {
            action = ACTION_TOGGLE;
        }
        long tlx = strtol(&line[ovector[4]], NULL, 10);
        long tly = strtol(&line[ovector[6]], NULL, 10);
        long brx = strtol(&line[ovector[8]], NULL, 10);
        long bry = strtol(&line[ovector[10]], NULL, 10);

        for (long x = tlx; x <= brx; ++x) {
            for (long y = tly; y <= bry; ++y) {
                switch (action) {
                case ACTION_TURN_ON:
                    grid[y][x] = true;
                    grid2[y][x]++;
                    break;
                case ACTION_TURN_OFF:
                    grid[y][x] = false;
                    if (grid2[y][x] > 0) {
                        grid2[y][x]--;
                    }
                    break;
                case ACTION_TOGGLE:
                    grid[y][x] = !grid[y][x];
                    grid2[y][x] += 2;
                    break;
                default:
                    break;
                }
            }
        }

        free(line);
        line = NULL;
    }
    free(line);

    size_t part1 = 0;
    size_t part2 = 0;
    for (size_t i = 0; i < 1000; ++i) {
        for (size_t j = 0; j < 1000; ++j) {
            part1 += grid[i][j];
            part2 += grid2[i][j];
        }
    }

    for (size_t i = 0; i < 1000; ++i) {
        free(grid[i]);
        free(grid2[i]);
    }
    free(grid);
    free(grid2);

    pcre2_match_data_free(matchData);
    fclose(fp);
    pcre2_code_free(linePattern);

    printf("Part 1: %zu\n", part1);
    printf("Part 2: %zu\n", part2);
    return 0;
}