#include <advent.h>
#include <config.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long add_all_numbers(json_object* obj) {
  long sum = 0;
  if (json_object_is_type(obj, json_type_object)) {
    json_object_object_foreach(obj, key, val) { sum += add_all_numbers(val); }
  } else if (json_object_is_type(obj, json_type_array)) {
    for (size_t i = 0; i < json_object_array_length(obj); i++) {
      json_object* val = json_object_array_get_idx(obj, i);
      sum += add_all_numbers(val);
    }
  } else if (json_object_is_type(obj, json_type_int)) {
    sum += json_object_get_int(obj);
  }
  return sum;
}

static bool is_red(json_object* obj) {
  if (json_object_is_type(obj, json_type_object)) {
    json_object_object_foreach(obj, key, val) {
      if (json_object_is_type(val, json_type_string) &&
          strcmp(json_object_get_string(val), "red") == 0) {
        return true;
      }
    }
  }
  return false;
}

static long add_all_non_red_numbers(json_object* obj) {
  long sum = 0;
  if (json_object_is_type(obj, json_type_object)) {
    if (!is_red(obj)) {
      json_object_object_foreach(obj, key, val) {
        sum += add_all_non_red_numbers(val);
      }
    }
  } else if (json_object_is_type(obj, json_type_array)) {
    for (size_t i = 0; i < json_object_array_length(obj); i++) {
      json_object* val = json_object_array_get_idx(obj, i);
      sum += add_all_non_red_numbers(val);
    }
  } else if (json_object_is_type(obj, json_type_int)) {
    sum += json_object_get_int(obj);
  }
  return sum;
}

int main(void) {
  char* text = advent_readEntireFile(ADVENT_INPUT);
  json_object* obj = json_tokener_parse(text);
  printf("%ld\n", add_all_numbers(obj));
  printf("%ld\n", add_all_non_red_numbers(obj));
  json_object_put(obj);
  free(text);
}
