#include <advent.h>
#include <config.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* s;
  size_t len;
} string_t;

static string_t increment_password(string_t old_password) {
  string_t new_password = {NULL, 0};
  new_password.len = old_password.len;
  new_password.s = advent_strdup(old_password.s);
  size_t pos = new_password.len - 1;
  new_password.s[pos]++;
  while (new_password.s[pos] > 'z') {
    new_password.s[pos] = 'a';
    if (pos == 0) {
      free(new_password.s);
      return (string_t){NULL, 0};
    }
    pos--;
    new_password.s[pos]++;
  }
  return new_password;
}

static bool has_doubles(string_t password) {
  for (size_t i = 0; i < password.len - 1; i++) {
    if (password.s[i] == password.s[i + 1]) {
      for (size_t j = i + 2; j < password.len - 1; j++) {
        if (password.s[j] == password.s[j + 1]) {
          return true;
        }
      }
    }
  }
  return false;
}

static bool has_straight(string_t password) {
  for (size_t i = 0; i < password.len - 2; i++) {
    if (password.s[i + 1] == password.s[i] + 1 &&
        password.s[i + 2] == password.s[i] + 2) {
      return true;
    }
  }
  return false;
}

static bool is_forbidden(string_t password) {
  for (size_t i = 0; i < password.len; i++) {
    if (password.s[i] == 'i' || password.s[i] == 'o' || password.s[i] == 'l') {
      return true;
    }
  }
  return false;
}

static bool is_valid(string_t password) {
  return password.len == 8 && has_doubles(password) && has_straight(password) &&
         !is_forbidden(password);
}

static string_t next_password(string_t password) {
  string_t new_password = increment_password(password);
  while (!is_valid(new_password)) {
    string_t old_password = new_password;
    new_password = increment_password(new_password);
    free(old_password.s);
  }
  return new_password;
}

int main(void) {
  char* raw_password = advent_readEntireFile(ADVENT_INPUT);
  string_t password = {
      .s = raw_password,
      .len = strlen(raw_password),
  };
  string_t np = next_password(password);
  printf("Next password: %s\n", np.s);
  string_t nnp = next_password(np);
  printf("Next-next password: %s\n", nnp.s);
  free(password.s);
  free(np.s);
  free(nnp.s);
}
