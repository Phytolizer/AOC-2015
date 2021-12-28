#include <advent.h>
#include <advent/pcre.h>
#include <advent/string.h>
#include <advent/vector.h>
#include <config.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_TIME = 2503 };

#define MAX(a, b)       \
  ({                    \
    typeof(a) a_ = (a); \
    typeof(b) b_ = (b); \
    a_ > b_ ? a_ : b_;  \
  })

typedef struct {
  string_t name;
  size_t speed;
  size_t fly_duration;
  size_t rest_duration;
  size_t position;
  size_t score;
} reindeer_t;

DECLARE_VECTOR(reindeer_vec_t, reindeer_t);
DEFINE_VECTOR(reindeer_vec_t, reindeer_t);

int reindeer_position_cmp(const void* a, const void* b, void* unused);
int reindeer_score_cmp(const void* a, const void* b, void* unused);

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  int err;
  size_t errofs;
  pcre2_code* code = pcre2_compile((PCRE2_SPTR)"(\\w+) can fly (\\d+) km/s for (\\d+) seconds, but then must rest for (\\d+) seconds\\.",
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

  char* line = NULL;
  size_t line_length;
  reindeer_vec_t reindeer;
  reindeer_vec_t_init(&reindeer);
  while (getline(&line, &line_length, fp) != -1) {
    char* end;
    if ((end = strrchr(line, '\n')) != NULL) {
      *end = '\0';
    } else {
      end = line + strlen(line);
    }

    int rc =
        pcre2_match(code, (PCRE2_SPTR)line, end - line, 0, 0, match_data, NULL);
    if (rc < 0) {
      printf("PCRE match failed!\n");
      return 1;
    }

    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
    reindeer_t r = {
        .name = string_from_cstr(
            advent_strndup(line + ovector[2], ovector[3] - ovector[2])),
        .speed = strtol(line + ovector[4], NULL, 10),
        .fly_duration = strtol(line + ovector[6], NULL, 10),
        .rest_duration = strtol(line + ovector[8], NULL, 10),
        .position = 0,
        .score = 0,
    };
    reindeer_vec_t_push(&reindeer, r);

    free(line);
    line = NULL;
  }
  free(line);

  size_t max_distance = 0;
  for (size_t second = 0; second < MAX_TIME; ++second) {
    for (size_t i = 0; i < reindeer.length; ++i) {
      reindeer_t* r = reindeer.data + i;
      if (second % (r->fly_duration + r->rest_duration) < r->fly_duration) {
        r->position += r->speed;
        max_distance = MAX(max_distance, r->position);
      }
    }
    advent_qsort_r(reindeer.data, reindeer.length, sizeof(reindeer_t),
                   reindeer_position_cmp, NULL);
    size_t position = reindeer.data[0].position;
    for (size_t i = 0; reindeer.data[i].position == position; ++i) {
      reindeer_t* r = reindeer.data + i;
      r->score++;
    }
  }

  advent_qsort_r(reindeer.data, reindeer.length, sizeof(reindeer_t),
                 reindeer_score_cmp, NULL);
  printf("== LEADERBOARD ==\n");
  for (size_t i = 0; i < reindeer.length; ++i) {
    printf("%s: %zu\n", reindeer.data[i].name.s, reindeer.data[i].score);
  }
  printf("\n");

  for (size_t i = 0; i < reindeer.length; ++i) {
    reindeer_t* r = reindeer.data + i;
    free(r->name.s);
  }
  reindeer_vec_t_deinit(&reindeer);

  return 0;
}

int reindeer_position_cmp(const void* a, const void* b, void* unused) {
  (void)unused;
  const reindeer_t* ra = a;
  const reindeer_t* rb = b;
  return rb->position - ra->position;
}

int reindeer_score_cmp(const void* a, const void* b, void* unused) {
  (void)unused;
  const reindeer_t* ra = a;
  const reindeer_t* rb = b;
  return rb->score - ra->score;
}
