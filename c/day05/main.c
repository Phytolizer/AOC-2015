#include <advent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isNice(char* line);
bool isNice2(char* line);

int main(void)
{
    FILE* fp = fopen(ADVENT_INPUT, "r");
    char* line = NULL;
    size_t lineLen = 0;
    size_t nnice = 0;
    size_t nnice2 = 0;
    while (getline(&line, &lineLen, fp) != -1)
    {
        if (isNice(line))
        {
            ++nnice;
        }
        if (isNice2(line))
        {
            ++nnice2;
        }

        free(line);
        line = NULL;
    }
    free(line);

    printf("part 1: %zu\n", nnice);
    printf("part 2: %zu\n", nnice2);

    fclose(fp);
    return 0;
}

bool isNice(char* line)
{
    bool hasDouble = false;
    bool hasBadPair = false;
    size_t nvowels = 0;
    char pc = ' ';
    for (char* c = line; *c != '\n' && *c != '\0'; ++c)
    {
        if (*c == 'a' || *c == 'e' || *c == 'i' || *c == 'o' || *c == 'u')
        {
            ++nvowels;
        }
        if (pc == 'a' && *c == 'b' || pc == 'c' && *c == 'd' || pc == 'p' && *c == 'q' || pc == 'x' && *c == 'y')
        {
            hasBadPair = true;
            break;
        }
        if (pc == *c)
        {
            hasDouble = true;
        }
        pc = *c;
    }

    return nvowels >= 3 && hasDouble && !hasBadPair;
}

bool hasPair(char* pool, char* pair)
{
    return strstr(pool, pair) != NULL;
}

bool isNice2(char* line)
{
    bool hasTwoPair = false;
    size_t lineLen = strlen(line) - 1; // strip \n
    for (int pairStartIdx = 0; pairStartIdx < lineLen - 1; ++pairStartIdx)
    {
        char* pair = malloc(3);
        strncpy(pair, &line[pairStartIdx], 2);
        pair[2] = '\0';
        if (pairStartIdx >= 2)
        {
            char* firstChunk = malloc(pairStartIdx + 1);
            strncpy(firstChunk, line, pairStartIdx);
            firstChunk[pairStartIdx] = '\0';
            if (hasPair(firstChunk, pair))
            {
                hasTwoPair = true;
            }
            free(firstChunk);
        }
        if (pairStartIdx < lineLen - 3)
        {
            size_t lastChunkLen = lineLen - (pairStartIdx + 2);
            char* lastChunk = malloc(lastChunkLen + 1);
            strncpy(lastChunk, &line[pairStartIdx + 2], lastChunkLen);
            lastChunk[lastChunkLen] = '\0';
            if (hasPair(lastChunk, pair))
            {
                hasTwoPair = true;
            }
            free(lastChunk);
        }
        free(pair);
    }
    bool hasSeparatedRepetition = false;
    for (size_t repStartIdx = 0; repStartIdx < lineLen - 2; ++repStartIdx)
    {
        if (line[repStartIdx] == line[repStartIdx + 2])
        {
            hasSeparatedRepetition = true;
            break;
        }
    }
    return hasTwoPair && hasSeparatedRepetition;
}
