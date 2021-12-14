#include <advent.h>
#include <stddef.h>
#include <stdlib.h>

int main(void)
{
    char* input = advent_readEntireFile("day01/input.txt");
    if (input == NULL)
    {
        return 1;
    }

    free(input);
    return 0;
}