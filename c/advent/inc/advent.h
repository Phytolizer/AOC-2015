#pragma once

#include <stddef.h>
#if __has_include("config.h")
#include "config.h"
#endif

#define NS_(x) advent_##x

#define ADVENT_INPUT CMAKE_CURRENT_SOURCE_DIR "/" CMAKE_TARGET "/input.txt"

char* NS_(readEntireFile)(const char* fileName);
long NS_(minElement)(long* begin, long* end);
char* NS_(allocatedSPrintf)(const char* format, ...);
char* NS_(strdup)(const char* str);
char* NS_(strndup)(const char* str, size_t n);
void* NS_(memdup)(const void* ptr, size_t size);
char* NS_(itoa)(int value);
void NS_(qsort_r)(void* base, size_t nmemb, size_t size,
                  int (*compar)(const void*, const void*, void*), void* arg);
