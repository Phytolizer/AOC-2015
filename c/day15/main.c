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

static size_t score_combo(ingredient_vec_t* ingredients,
                          advent_combinatorics_sum_comb_t* comb);

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

  size_t best_score = 0;

  advent_combinatorics_sum_comb_t comb =
      advent_combinatorics_first_sum_comb(ingredients.length, 100);
  do {
    size_t combo = score_combo(&ingredients, &comb);
    if (combo > best_score) {
      best_score = combo;
    }
  } while (advent_combinatorics_next_sum_comb(&comb, 100));
  advent_combinatorics_sum_comb_t_deinit(&comb);

  printf("%zu\n", best_score);

  free(line);
  fclose(fp);
  pcre2_match_data_free(match_data);
  pcre2_code_free(code);
  for (size_t i = 0; i < ingredients.length; ++i) {
    string_free(ingredients.data[i].name);
  }
  ingredient_vec_t_deinit(&ingredients);
  return 0;
}

static size_t score_combo(ingredient_vec_t* ingredients,
                          advent_combinatorics_sum_comb_t* comb) {
  ingredient_t resulting_combo = {0};
  for (size_t i = 0; i < ingredients->length; i++) {
    ingredient_t* ingredient = &ingredients->data[i];
    int amount = comb->data[i];
    resulting_combo.capacity += ingredient->capacity * amount;
    resulting_combo.durability += ingredient->durability * amount;
    resulting_combo.flavor += ingredient->flavor * amount;
    resulting_combo.texture += ingredient->texture * amount;
    resulting_combo.calories += ingredient->calories * amount;
  }

  if (resulting_combo.capacity < 0 || resulting_combo.durability < 0 ||
      resulting_combo.flavor < 0 || resulting_combo.texture < 0) {
    return 0;
  }
  if (resulting_combo.calories != 500) {
    return 0;
  }

  return resulting_combo.capacity * resulting_combo.durability *
         resulting_combo.flavor * resulting_combo.texture;
}
