#pragma once

#include <stdbool.h>

#include "advent/vector.h"

DECLARE_VECTOR(advent_combinatorics_sum_comb_t, int);

advent_combinatorics_sum_comb_t advent_combinatorics_first_sum_comb(int n,
                                                                    int sum);
bool advent_combinatorics_next_sum_comb(advent_combinatorics_sum_comb_t* comb,
                                        int sum);
