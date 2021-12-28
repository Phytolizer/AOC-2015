#include <advent.h>
#include <advent/combinatorics.h>
#include <advent/vector.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DECLARE_VECTOR(int_vec_t, int);
DEFINE_VECTOR(int_vec_t, int);

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  char* line = NULL;
  size_t line_length = 0;
  int_vec_t container_sizes;
  int_vec_t_init(&container_sizes);
  while (getline(&line, &line_length, fp) != -1) {
    int_vec_t_push(&container_sizes, strtol(line, NULL, 10));
  }

  size_t counter = 0;
  size_t min_count = 0;
  size_t min_size = SIZE_MAX;
  for (size_t r = 1; r < container_sizes.length; ++r) {
    advent_combinatorics_comb_t comb =
        advent_combinatorics_first_comb(container_sizes.length, r);
    do {
      size_t sum = 0;
      for (size_t i = 0; i < comb.length; ++i) {
        sum += container_sizes.data[comb.data[i]];
      }
      if (sum == 150) {
        ++counter;
        if (comb.length < min_size) {
          min_size = comb.length;
          min_count = 1;
        } else if (comb.length == min_size) {
          ++min_count;
        }
      }
    } while (advent_combinatorics_next_comb(&comb, container_sizes.length));
  }
  printf("%zu\n", counter);
  printf("%zu\n", min_count);
  free(line);
}
