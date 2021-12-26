#include "advent/set.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SET_MAX_LOAD 0.75
#define SET_NEXT_CAPACITY(c) ((c) == 0 ? 8 : (c)*2)

#define FNV_OFFSET_BASIS 0xCBF29CE484222325
#define FNV_PRIME 0x00000100000001B3

static SetBucket* findBucket(SetBucket* data, size_t length, const void* value,
                             size_t valueLength);
static uint64_t hash(const void* value, size_t valueLength);

void Set_init(Set* s) {
  s->data = NULL;
  s->count = 0;
  s->capacity = 0;
}

void Set_deinit(Set* s) {
  for (size_t i = 0; i < s->capacity; ++i) {
    if (s->data[i].value != NULL) {
      free(s->data[i].value);
    }
  }
  free(s->data);
}

bool Set_insert(Set* s, const void* value, size_t valueLength) {
  if (s->capacity == 0 ||
      (double)s->count / (double)s->capacity > SET_MAX_LOAD) {
    size_t oldCapacity = s->capacity;
    s->capacity = SET_NEXT_CAPACITY(s->capacity);
    SetBucket* newData = malloc(s->capacity * sizeof(SetBucket));
    if (newData == NULL) {
      fprintf(stderr, "Fatal error: malloc failure\n");
      exit(EXIT_FAILURE);
    }
    memset(newData, 0, s->capacity * sizeof(SetBucket));
    for (size_t i = 0; i < oldCapacity; ++i) {
      if (s->data[i].value != NULL) {
        SetBucket* bucket = findBucket(newData, s->capacity, s->data[i].value,
                                       s->data[i].valueLength);
        bucket->value = s->data[i].value;
        bucket->valueLength = s->data[i].valueLength;
      }
    }
    free(s->data);
    s->data = newData;
  }

  SetBucket* bucket = findBucket(s->data, s->capacity, value, valueLength);
  bool ret = false;
  if (bucket->value != NULL) {
    free(bucket->value);
    ret = true;
  }
  bucket->value = malloc(valueLength);
  memcpy(bucket->value, value, valueLength);
  bucket->valueLength = valueLength;
  ++s->count;
  return ret;
}

bool Set_includes(Set* s, const void* value, size_t valueLength) {
  SetBucket* bucket = findBucket(s->data, s->capacity, value, valueLength);
  return bucket->value != NULL;
}

static uint64_t hash(const void* value, size_t valueLength) {
  uint64_t h = FNV_OFFSET_BASIS;
  const uint8_t* valueC = value;
  for (size_t i = 0; i < valueLength; ++i) {
    h ^= valueC[i];
    h *= FNV_PRIME;
  }
  return h;
}

static SetBucket* findBucket(SetBucket* data, size_t length, const void* value,
                             size_t valueLength) {
  uint64_t h = hash(value, valueLength);
  h %= length;
  while (data[h].value != NULL &&
         !(data[h].valueLength == valueLength &&
           memcmp(data[h].value, value, valueLength) == 0)) {
    h = (h + 1) % length;
  }
  return &data[h];
}
