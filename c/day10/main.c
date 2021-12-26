#include <advent.h>
#include <advent/vector.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DECLARE_VECTOR(string_t, char);
DEFINE_VECTOR(string_t, char);

static string_t look_and_say(char* text);

int main(void) {
  char* text = advent_readEntireFile(ADVENT_INPUT);

  for (size_t i = 0; i < 50; ++i) {
    string_t next = look_and_say(text);
    free(text);
    text = next.data;
  }
  printf("%zu\n", strlen(text));

  free(text);
  return 0;
}

static string_t look_and_say(char* text) {
  string_t result;
  string_t_init(&result);

  size_t count = 0;
  char prev = text[0];
  for (char* c = text; *c != '\0'; c++) {
    if (*c == prev) {
      count++;
    } else {
      char buf[32];
      snprintf(buf, sizeof(buf), "%zu%c", count, prev);
      for (char* bc = buf; *bc != '\0'; bc++) {
        string_t_push(&result, *bc);
      }
      count = 1;
      prev = *c;
    }
  }
  char buf[32];
  snprintf(buf, sizeof(buf), "%zu%c", count, prev);
  for (char* bc = buf; *bc != '\0'; bc++) {
    string_t_push(&result, *bc);
  }
  string_t_push(&result, '\0');

  return result;
}
