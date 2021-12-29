#include <advent.h>
#include <advent/vector.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DECLARE_VECTOR(light_field_line_t, char);
DECLARE_VECTOR(light_field_t, light_field_line_t);
DEFINE_VECTOR(light_field_line_t, char);
DEFINE_VECTOR(light_field_t, light_field_line_t);

#define MAX(a, b)       \
  ({                    \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b;  \
  })

size_t count_neighbors(light_field_t* field, size_t x, size_t y) {
  size_t count = 0;
  for (size_t i = MAX(x, 1) - 1; i <= x + 1; ++i) {
    for (size_t j = MAX(y, 1) - 1; j <= y + 1; ++j) {
      if (i == x && j == y) {
        continue;
      }
      if (i < field->length && j < field->data[i].length &&
          field->data[i].data[j] == '#') {
        ++count;
      }
    }
  }
  return count;
}

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  light_field_t lights;
  light_field_t_init(&lights);
  char* line = NULL;
  size_t line_length = 0;
  while (getline(&line, &line_length, fp) != -1) {
    light_field_line_t light_line;
    light_field_line_t_init(&light_line);
    for (char* c = line; *c != '\0' && *c != '\n'; ++c) {
      light_field_line_t_push(&light_line, *c);
    }
    light_field_t_push(&lights, light_line);
  }

  light_field_t temp_lights;
  light_field_t_init(&temp_lights);
  for (size_t i = 0; i < lights.length; ++i) {
    light_field_line_t temp_line = light_field_line_t_dup(&lights.data[i]);
    light_field_t_push(&temp_lights, temp_line);
  }

  for (size_t i = 0; i < 100; ++i) {
    for (size_t x = 0; x < lights.length; ++x) {
      for (size_t y = 0; y < lights.data[x].length; ++y) {
        size_t count = count_neighbors(&lights, x, y);
        if (lights.data[x].data[y] == '#' && count != 2 && count != 3) {
          temp_lights.data[x].data[y] = '.';
        } else if (lights.data[x].data[y] == '.' && count == 3) {
          temp_lights.data[x].data[y] = '#';
        } else {
          temp_lights.data[x].data[y] = lights.data[x].data[y];
        }
      }
    }
    light_field_t temp = lights;
    lights = temp_lights;
    temp_lights = temp;
  }

  size_t count = 0;
  for (size_t x = 0; x < lights.length; ++x) {
    for (size_t y = 0; y < lights.data[x].length; ++y) {
      if (lights.data[x].data[y] == '#') {
        ++count;
      }
    }
  }

  printf("%zu\n", count);

  for (size_t i = 0; i < lights.length; ++i) {
    light_field_line_t_deinit(&lights.data[i]);
  }
  light_field_t_deinit(&lights);
  free(line);
  fclose(fp);
  return 0;
}
