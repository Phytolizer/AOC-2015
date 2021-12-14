#include "advent.h"
#include <advent/set.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    MOVER_SANTA,
    MOVER_ROBO_SANTA,
} WhichMover;

typedef struct
{
    long x;
    long y;
} Mover;

int main(void)
{
    Set visited;
    Set_init(&visited);
    Set visited2;
    Set_init(&visited2);

    Mover part1 = {0, 0};
    Mover santa = {0, 0};
    Mover roboSanta = {0, 0};
    WhichMover mover = MOVER_SANTA;
    Set_insert(&visited, &part1, sizeof(Mover));
    Set_insert(&visited2, &santa, sizeof(Mover));

    size_t nVisited = 1;
    size_t nVisited2 = 1;

    char* input = advent_readEntireFile(ADVENT_INPUT);

    for (char* c = input; *c != '\0'; ++c)
    {
        Mover* mx;
        switch (mover)
        {
        case MOVER_SANTA:
            mx = &santa;
            break;
        case MOVER_ROBO_SANTA:
            mx = &roboSanta;
            break;
        default:
            break;
        }

        switch (*c)
        {
        case '^':
            ++mx->y;
            ++part1.y;
            break;
        case 'v':
            --mx->y;
            --part1.y;
            break;
        case '<':
            --mx->x;
            --part1.x;
            break;
        case '>':
            ++mx->x;
            ++part1.x;
            break;
        }

        if (!Set_insert(&visited, &part1, sizeof(Mover)))
        {
            ++nVisited;
        }
        if (!Set_insert(&visited2, mx, sizeof(Mover)))
        {
            ++nVisited2;
        }
        switch (mover)
        {
        case MOVER_SANTA:
            mover = MOVER_ROBO_SANTA;
            break;
        case MOVER_ROBO_SANTA:
            mover = MOVER_SANTA;
            break;
        default:
            break;
        }
    }

    printf("Part 1: %zu\n", nVisited);
    printf("Part 2: %zu\n", nVisited2);

    free(input);
    Set_deinit(&visited2);
    Set_deinit(&visited);
}
