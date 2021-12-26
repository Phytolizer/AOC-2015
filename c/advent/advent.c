#include "advent.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char* NS_(readEntireFile)(const char* fileName) {
  FILE* fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open %s\n", fileName);
    return NULL;
  }
  fseek(fp, 0, SEEK_END);
  long fileLen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* result = malloc(fileLen + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for %s contents\n", fileName);
    return NULL;
  }
  fread(result, sizeof(char), fileLen, fp);
  if (ferror(fp)) {
    fprintf(stderr, "Failed while reading %s\n", fileName);
    free(result);
    return NULL;
  }
  result[fileLen] = '\0';
  return result;
}
long NS_(minElement)(long* begin, long* end) {
  long min = *begin;
  for (long* p = begin; p != end; ++p) {
    if (*p < min) {
      min = *p;
    }
  }
  return min;
}
char* NS_(allocatedSPrintf)(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(NULL, 0, format, args);
  va_end(args);
  char* result = malloc(len + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  va_start(args, format);
  vsnprintf(result, len + 1, format, args);
  va_end(args);
  return result;
}
char* NS_(strdup)(const char* str) {
  if (str == NULL) {
    return NULL;
  }
  char* result = malloc(strlen(str) + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  strcpy(result, str);
  return result;
}
char* NS_(strndup)(const char* str, size_t n) {
  if (str == NULL) {
    return NULL;
  }
  char* result = malloc(n + 1);
  memcpy(result, str, n);
  result[n] = '\0';
  return result;
}
void* NS_(memdup)(const void* ptr, size_t size) {
  void* result = malloc(size);
  memcpy(result, ptr, size);
  return result;
}
char* NS_(itoa)(int value) {
  char* result = malloc(32);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  sprintf(result, "%d", value);
  return result;
}
