#include "advent/combinatorics.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFINE_VECTOR(advent_combinatorics_sum_comb_t, int);
DEFINE_VECTOR(advent_combinatorics_comb_t, size_t);

static bool check_limit(advent_combinatorics_sum_comb_t* comb, size_t n,
                        int limit);

advent_combinatorics_sum_comb_t advent_combinatorics_first_sum_comb(int n,
                                                                    int sum) {
    advent_combinatorics_sum_comb_t result;
    advent_combinatorics_sum_comb_t_init(&result);
    if (n == 0) {
        return result;
    }
    if (n == 1) {
        advent_combinatorics_sum_comb_t_push(&result, sum);
        return result;
    }
    for (size_t i = 0; i < n - 1; ++i) {
        advent_combinatorics_sum_comb_t_push(&result, 1);
    }
    advent_combinatorics_sum_comb_t_push(&result, sum - (n - 1));
    return result;
}

bool advent_combinatorics_next_sum_comb(advent_combinatorics_sum_comb_t* comb,
                                        int sum) {
    // the final combination will start with a large number and end with all 1s.
    // the second half of the || checks for this case.
    if (comb->length <= 1 || comb->data[0] >= sum - (comb->length - 1)) {
        return false;
    }
    size_t i = comb->length - 2;
    // due to the above check, an infinite loop will definitely terminate.
    //
    // go through each prior element and wrap around if necessary.
    while (true) {
        comb->data[i]++;
        // the max sum of comb[..i] is sum - (comb.length - 1 - i).
        // this is assuming comb[i + 1..] is all 1s, which it will be by the
        // time the limit is reached.
        if (!check_limit(comb, i + 1, sum - (comb->length - 1 - i))) {
            // if the sum is less than that, then we have found the next
            // combination. update last element, it's always sum -
            // sum(comb[..-1]).
            int partial = 0;
            for (size_t j = 0; j < comb->length - 1; ++j) {
                partial += comb->data[j];
            }
            comb->data[comb->length - 1] = sum - partial;
            // because of constraints we expressed by calling check_limit, we
            // know that sum - partial is positive.
            return true;
        }
        // we hit a limit -- wrap back to 1 and move on to the next element.
        comb->data[i] = 1;
        --i;
    }
}

advent_combinatorics_comb_t advent_combinatorics_first_comb(int n, int r) {
    advent_combinatorics_comb_t result;
    advent_combinatorics_comb_t_init(&result);
    if (r == 0 || r > n) {
        return result;
    }
    for (size_t i = 0; i < r; ++i) {
        advent_combinatorics_comb_t_push(&result, i);
    }
    return result;
}

bool advent_combinatorics_next_comb(advent_combinatorics_comb_t* comb, int n) {
    bool found = false;
    size_t i;
    for (i = comb->length - 1;; --i) {
        if (comb->data[i] != i + n - comb->length) {
            found = true;
            break;
        }
        if (i == 0) {
            break;
        }
    }
    if (!found) {
        return false;
    }
    comb->data[i]++;
    for (size_t j = i + 1; j < comb->length; ++j) {
        comb->data[j] = comb->data[j - 1] + 1;
    }
    return true;
}

static bool check_limit(advent_combinatorics_sum_comb_t* comb, size_t n,
                        int limit) {
    int accum = 0;
    for (size_t i = 0; i < n; ++i) {
        accum += comb->data[i];
    }
    return accum > limit;
}
