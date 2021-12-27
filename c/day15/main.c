#include <advent.h>
#include <advent/combinatorics.h>
#include <advent/pcre.h>
#include <advent/string.h>
#include <advent/vector.h>
#include <config.h>
#include <pcre2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  string_t name;
  int capacity;
  int durability;
  int flavor;
  int texture;
  int calories;
} ingredient_t;

DECLARE_VECTOR(ingredient_vec_t, ingredient_t);
DEFINE_VECTOR(ingredient_vec_t, ingredient_t);

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  int err;
  size_t errofs;
  pcre2_code* code = pcre2_compile((PCRE2_SPTR)"(\\w+): capacity (-?\\d+), durability (-?\\d+), flavor (-?\\d+), texture (-?\\d+), calories (-?\\d+)",
                                   PCRE2_ZERO_TERMINATED, 0, &err, &errofs, NULL);
  if (code == NULL) {
    PCRE2_UCHAR8 buffer[256];
    pcre2_get_error_message(err, buffer, sizeof(buffer));
    printf("PCRE compilation failed at offset %zu: %s\n", errofs, buffer);
    return 1;
  }

  pcre2_match_data* match_data =
      pcre2_match_data_create_from_pattern(code, NULL);
  if (match_data == NULL) {
    printf("Failed to create match data!\n");
    return 1;
  }

  ingredient_vec_t ingredients;
  ingredient_vec_t_init(&ingredients);

  char* line = NULL;
  size_t line_length = 0;
  while (getline(&line, &line_length, fp) != -1) {
    int rc = pcre2_match(code, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED, 0, 0,
                         match_data, NULL);
    if (rc < 0) {
      printf("Error matching: %d\n", rc);
      return 1;
    }

    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
    ingredient_t ingredient = {
        .name = string_from_cstr(
            advent_strndup(line + ovector[2], ovector[3] - ovector[2])),
        .capacity = strtol(line + ovector[4], NULL, 10),
        .durability = strtol(line + ovector[6], NULL, 10),
        .flavor = strtol(line + ovector[8], NULL, 10),
        .texture = strtol(line + ovector[10], NULL, 10),
        .calories = strtol(line + ovector[12], NULL, 10),
    };
    ingredient_vec_t_push(&ingredients, ingredient);
  }

  advent_combinatorics_sum_comb_t comb =
      advent_combinatorics_first_sum_comb(4, 8);
  do {
    for (size_t i = 0; i < 4; ++i) {
      printf("%d ", comb.data[i]);
    }
    printf("\n");
  } while (advent_combinatorics_next_sum_comb(&comb, 8));

  return 0;
}
