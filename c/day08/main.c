#include <advent/pcre.h>
#include <assert.h>
#include <config.h>
#include <pcre2.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  int pcre2_error;
  size_t pcre2_error_offset;
  pcre2_code* code = pcre2_compile(
      (PCRE2_SPTR) "[^\\\\]|\\\\(?:x([0-9a-fA-F]{2})|[\\\\\"])",
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
  size_t decoded_length = 0;
  while (getline(&line, &line_length, input) != EOF) {
    char* interior_line = &line[1];
    char* interior_line_end = strrchr(interior_line, '"');
    assert(interior_line_end != NULL && "No closing quote");
    original_length += interior_line_end - interior_line + 2;
    size_t line_length_without_endquote = interior_line_end - interior_line + 1;
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
      decoded_length++;
    }
    free(line);
    line = NULL;
  }
  free(line);
  printf("%zu\n", original_length - decoded_length);

  pcre2_match_data_free(match_data);
  pcre2_code_free(code);
  return 0;
}
