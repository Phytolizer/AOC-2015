#include <advent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char* input = advent_readEntireFile(ADVENT_INPUT);
    if (input == NULL) {
        return 1;
    }

    int floor = 0;
    size_t index = 1;
    bool indexFound = false;

    for (char* c = input; *c != '\0'; c++) {
        switch (*c) {
        case '(':
            ++floor;
            break;
        case ')':
            --floor;
            if (!indexFound && floor < 0) {
                indexFound = true;
                printf("Part 2: %zu\n", index);
            }
            break;
        default:
            break;
        }
        ++index;
    }
    printf("Part 1: %d\n", floor);

    free(input);
    return 0;
}