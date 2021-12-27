#include "advent/combinatorics.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

DEFINE_VECTOR(advent_combinatorics_sum_comb_t, int);

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
  if (comb->length <= 1 || comb->data[0] >= sum - (comb->length - 1)) {
    return false;
  }
  size_t i = comb->length - 2;
  // Due to the above check, an infinite loop will definitely terminate.
  while (true) {
    comb->data[i]++;
    if (!check_limit(comb, i + 1, sum)) {
      // update last element
      int partial = 0;
      for (size_t j = 0; j < comb->length - 1; ++j) {
        partial += comb->data[j];
      }
      comb->data[comb->length - 1] = sum - partial;
      if (sum - partial <= 0) {
        // numbers got too large, try again
        return advent_combinatorics_next_sum_comb(comb, sum);
      }
      return true;
    }
    comb->data[i] = 1;
    --i;
  }
}

static bool check_limit(advent_combinatorics_sum_comb_t* comb, size_t n,
                        int limit) {
  int accum = 0;
  for (size_t i = 0; i < n; ++i) {
    accum += comb->data[i];
  }
  return accum > limit;
}
