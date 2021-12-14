#include "advent.h"
#include <malloc.h>
#include <stdio.h>

char* NS_(readEntireFile)(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to open %s\n", fileName);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* result = malloc(fileLen + 1);
    if (!result)
    {
        fprintf(stderr, "Failed allocating memory for %s contents\n", fileName);
        return NULL;
    }
    fread(result, sizeof(char), fileLen, fp);
    if (ferror(fp))
    {
        fprintf(stderr, "Failed while reading %s\n", fileName);
        free(result);
        return NULL;
    }
    result[fileLen] = '\0';
    return result;
}
long NS_(minElement)(long* begin, long* end)
{
    long min = *begin;
    for (long* p = begin; p != end; ++p)
    {
        if (*p < min)
        {
            min = *p;
        }
    }
    return min;
}
