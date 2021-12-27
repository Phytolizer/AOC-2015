#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "advent/vector.h"

DECLARE_VECTOR(advent_combinatorics_sum_comb_t, int);
DECLARE_VECTOR(advent_combinatorics_comb_t, size_t);

advent_combinatorics_sum_comb_t advent_combinatorics_first_sum_comb(int n,
                                                                    int sum);
bool advent_combinatorics_next_sum_comb(advent_combinatorics_sum_comb_t* comb,
                                        int sum);

advent_combinatorics_comb_t advent_combinatorics_first_comb(int n, int r);
bool advent_combinatorics_next_comb(advent_combinatorics_comb_t* comb, int n);
