#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void* value;
  size_t valueLength;
} SetBucket;

typedef struct {
  SetBucket* data;
  size_t count;
  size_t capacity;
} Set;

void Set_init(Set* s);
void Set_deinit(Set* s);
bool Set_insert(Set* s, const void* value, size_t valueLength);
bool Set_includes(Set* s, const void* value, size_t valueLength);
