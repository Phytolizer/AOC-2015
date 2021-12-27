#include <advent.h>
#include <advent/combinatorics.h>
#include <advent/vector.h>
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
      }
    } while (advent_combinatorics_next_comb(&comb, container_sizes.length));
  }
  printf("%zu\n", counter);
  free(line);
}
