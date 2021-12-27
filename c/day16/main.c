#include <advent.h>
#include <advent/pcre.h>
#include <advent/vector.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDK (-1)

typedef struct {
  int which;
  int children;
  int cats;
  int samoyeds;
  int pomeranians;
  int akitas;
  int vizslas;
  int goldfish;
  int trees;
  int cars;
  int perfumes;
} sue_t;

DECLARE_VECTOR(sue_vec_t, sue_t);
DEFINE_VECTOR(sue_vec_t, sue_t);

void sue_init(sue_t* sue, int which) {
  sue->which = which;
  sue->children = IDK;
  sue->cats = IDK;
  sue->samoyeds = IDK;
  sue->pomeranians = IDK;
  sue->akitas = IDK;
  sue->vizslas = IDK;
  sue->goldfish = IDK;
  sue->trees = IDK;
  sue->cars = IDK;
  sue->perfumes = IDK;
}

bool sue_match(sue_t a, sue_t b) {
  return (a.children == IDK || a.children == b.children) &&
         (a.cats == IDK || a.cats == b.cats) &&
         (a.samoyeds == IDK || a.samoyeds == b.samoyeds) &&
         (a.pomeranians == IDK || a.pomeranians == b.pomeranians) &&
         (a.akitas == IDK || a.akitas == b.akitas) &&
         (a.vizslas == IDK || a.vizslas == b.vizslas) &&
         (a.goldfish == IDK || a.goldfish == b.goldfish) &&
         (a.trees == IDK || a.trees == b.trees) &&
         (a.cars == IDK || a.cars == b.cars) &&
         (a.perfumes == IDK || a.perfumes == b.perfumes);
}

bool sue_match_2(sue_t sue, sue_t given) {
  return (sue.children == IDK || sue.children == given.children) &&
         (sue.cats == IDK || sue.cats > given.cats) &&
         (sue.samoyeds == IDK || sue.samoyeds == given.samoyeds) &&
         (sue.pomeranians == IDK || sue.pomeranians < given.pomeranians) &&
         (sue.akitas == IDK || sue.akitas == given.akitas) &&
         (sue.vizslas == IDK || sue.vizslas == given.vizslas) &&
         (sue.goldfish == IDK || sue.goldfish < given.goldfish) &&
         (sue.trees == IDK || sue.trees > given.trees) &&
         (sue.cars == IDK || sue.cars == given.cars) &&
         (sue.perfumes == IDK || sue.perfumes == given.perfumes);
}

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  int err;
  size_t errofs;
  pcre2_code* line_code =
      pcre2_compile((PCRE2_SPTR) "Sue (\\d+): (.*)", PCRE2_ZERO_TERMINATED, 0,
                    &err, &errofs, NULL);
  if (line_code == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(err, buffer, sizeof(buffer));
    printf("PCRE2 compilation failed: %s\n", buffer);
    return 1;
  }

  pcre2_match_data* line_match_data =
      pcre2_match_data_create_from_pattern(line_code, NULL);
  if (line_match_data == NULL) {
    printf("Failed to create match data!\n");
    return 1;
  }

  pcre2_code* sue_code =
      pcre2_compile((PCRE2_SPTR) "(\\w+): (\\d+)", PCRE2_ZERO_TERMINATED, 0,
                    &err, &errofs, NULL);
  if (sue_code == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(err, buffer, sizeof(buffer));
    printf("PCRE2 compilation failed: %s\n", buffer);
    return 1;
  }

  pcre2_match_data* sue_match_data =
      pcre2_match_data_create_from_pattern(sue_code, NULL);
  if (sue_match_data == NULL) {
    printf("Failed to create match data!\n");
    return 1;
  }

  sue_vec_t sues;
  sue_vec_t_init(&sues);

  sue_t given = {
      .which = IDK,
      .children = 3,
      .cats = 7,
      .samoyeds = 2,
      .pomeranians = 3,
      .akitas = 0,
      .vizslas = 0,
      .goldfish = 5,
      .trees = 3,
      .cars = 2,
      .perfumes = 1,
  };

  char* line = NULL;
  size_t line_length = 0;
  while (getline(&line, &line_length, fp) != -1) {
    if (pcre2_match(line_code, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED, 0, 0,
                    line_match_data, NULL) < 0) {
      printf("Failed to match line!\n");
      return 1;
    }

    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(line_match_data);
    sue_t sue;
    sue_init(&sue, strtol(line + ovector[2], NULL, 10));

    PCRE2_SIZE offset = ovector[4];
    while (pcre2_match(sue_code, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED,
                       offset, 0, sue_match_data, NULL) > 0) {
      PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(sue_match_data);
      char* name = advent_strndup(line + ovector[2], ovector[3] - ovector[2]);
      char* value = line + ovector[4];

      if (strcmp(name, "children") == 0) {
        sue.children = strtol(value, NULL, 10);
      } else if (strcmp(name, "cats") == 0) {
        sue.cats = strtol(value, NULL, 10);
      } else if (strcmp(name, "samoyeds") == 0) {
        sue.samoyeds = strtol(value, NULL, 10);
      } else if (strcmp(name, "pomeranians") == 0) {
        sue.pomeranians = strtol(value, NULL, 10);
      } else if (strcmp(name, "akitas") == 0) {
        sue.akitas = strtol(value, NULL, 10);
      } else if (strcmp(name, "vizslas") == 0) {
        sue.vizslas = strtol(value, NULL, 10);
      } else if (strcmp(name, "goldfish") == 0) {
        sue.goldfish = strtol(value, NULL, 10);
      } else if (strcmp(name, "trees") == 0) {
        sue.trees = strtol(value, NULL, 10);
      } else if (strcmp(name, "cars") == 0) {
        sue.cars = strtol(value, NULL, 10);
      } else if (strcmp(name, "perfumes") == 0) {
        sue.perfumes = strtol(value, NULL, 10);
      }
      free(name);
      offset = ovector[1];
    }

    if (sue_match(sue, given)) {
      printf("Sue %d is the one!\n", sue.which);
    }
    if (sue_match_2(sue, given)) {
      printf("Sue %d is actually the one!\n", sue.which);
    }
    sue_vec_t_push(&sues, sue);
  }

  sue_vec_t_deinit(&sues);
  pcre2_code_free(line_code);
  pcre2_match_data_free(line_match_data);
  pcre2_code_free(sue_code);
  pcre2_match_data_free(sue_match_data);
  return 0;
}
