#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "advent/vector.h"

typedef struct {
  size_t index;
  int direction;
} permutation_element_t;

DECLARE_VECTOR(permutation_t, permutation_element_t);
DECLARE_VECTOR(permutation_vec_t, permutation_t);

permutation_t first_permutation(size_t n);
bool next_permutation(permutation_t* p);
permutation_element_t* permutation_swap(permutation_t* permutation, size_t i);
