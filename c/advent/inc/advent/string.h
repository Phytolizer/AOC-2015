#pragma once

#include <stddef.h>

typedef struct {
  char* s;
  size_t l;
} string_t;

string_t string_from_cstr(char* s);
void string_free(string_t s);
