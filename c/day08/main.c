#include <advent/pcre.h>
#include <advent/vector.h>
#include <assert.h>
#include <config.h>
#include <pcre2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  dtReg,
  dtEsc,
  dtByte,
} decoded_type_t;

typedef struct {
  decoded_type_t t;
  union {
    char reg;
    char esc;
    uint8_t byte;
  } u;
} decoded_char_t;

DECLARE_VECTOR(decoded_t, decoded_char_t);
DEFINE_VECTOR(decoded_t, decoded_char_t);

DECLARE_VECTOR(decoded_vec_t, decoded_t);
DEFINE_VECTOR(decoded_vec_t, decoded_t);

int main(void) {
  int pcre2_error;
  size_t pcre2_error_offset;
  pcre2_code* code = pcre2_compile(
      (PCRE2_SPTR) "[^\\\\]|\\\\(?:x([0-9a-fA-F]{2})|([\\\\\"]))",
      PCRE2_ZERO_TERMINATED, 0, &pcre2_error, &pcre2_error_offset, NULL);
  if (code == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(pcre2_error, buffer, sizeof(buffer));
    fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n",
            (int)pcre2_error_offset, buffer);
    return 1;
  }

  pcre2_match_data* match_data =
      pcre2_match_data_create_from_pattern(code, NULL);
  if (match_data == NULL) {
    fprintf(stderr, "Failed to create match data\n");
    return 1;
  }

  const char* path = CMAKE_CURRENT_SOURCE_DIR "/" CMAKE_TARGET "/input.txt";
  FILE* input = fopen(path, "r");
  if (input == NULL) {
    fprintf(stderr, "Failed to open input file: %s\n", path);
    return 1;
  }

  char* line = NULL;
  size_t line_length = 0;
  size_t original_length = 0;
  decoded_vec_t decoded;
  decoded_vec_t_init(&decoded);
  size_t decoded_length = 0;
  while (getline(&line, &line_length, input) != EOF) {
    char* interior_line = &line[1];
    char* interior_line_end = strrchr(interior_line, '"');
    assert(interior_line_end != NULL && "No closing quote");
    original_length += interior_line_end - interior_line + 2;
    size_t line_length_without_endquote = interior_line_end - interior_line + 1;
    decoded_t decoded_line;
    decoded_t_init(&decoded_line);
    for (size_t begin = 1; begin < line_length_without_endquote;) {
      int result =
          pcre2_match(code, (PCRE2_SPTR)line, line_length_without_endquote,
                      begin, 0, match_data, NULL);
      if (result < 0) {
        fprintf(stderr, "Failed to match\n");
        return 1;
      }
      PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
      begin += (int)ovector[1] - (int)ovector[0];
      if (ovector[5] - ovector[4] > 0) {
        decoded_char_t d = {
            .t = dtEsc,
            .u.esc = line[ovector[4]],
        };
        decoded_t_push(&decoded_line, d);
      } else if (ovector[3] - ovector[2] == 2) {
        char raw[3] = {line[ovector[2]], line[ovector[2] + 1], '\0'};
        decoded_char_t d = {
            .t = dtByte,
            .u.byte = (uint8_t)strtol(raw, NULL, 16),
        };
        decoded_t_push(&decoded_line, d);
      } else {
        decoded_char_t d = {
            .t = dtReg,
            .u.reg = line[ovector[0]],
        };
        decoded_t_push(&decoded_line, d);
      }
      ++decoded_length;
    }
    decoded_vec_t_push(&decoded, decoded_line);
    free(line);
    line = NULL;
  }
  free(line);
  printf("%zu\n", original_length - decoded_length);

  fclose(input);

  size_t re_encoded_length = 0;
  for (size_t i = 0; i < decoded.length; ++i) {
    // printf("\"\\\"");
    re_encoded_length += 3;
    for (size_t j = 0; j < decoded.data[i].length; ++j) {
      decoded_char_t* c = &decoded.data[i].data[j];
      switch (c->t) {
        case dtReg:
          switch (c->u.reg) {
            case '"':
            case '\\':
              re_encoded_length += 2;
              // printf("\\%c", c->u.reg);
              break;
            default:
              ++re_encoded_length;
              // printf("%c", c->u.reg);
              break;
          }
          break;
        case dtEsc:
          re_encoded_length += 4;
          // printf("\\\\\\%c", c->u.esc);
          break;
        case dtByte:
          re_encoded_length += 5;
          // printf("\\\\x%02x", c->u.byte);
          break;
      }
    }
    // printf("\\\"\"\n");
    re_encoded_length += 3;
  }

  printf("%zu\n", re_encoded_length - original_length);

  for (size_t i = 0; i < decoded.length; ++i) {
    decoded_t_deinit(&decoded.data[i]);
  }
  decoded_vec_t_deinit(&decoded);
  pcre2_match_data_free(match_data);
  pcre2_code_free(code);
  return 0;
}
