#include <advent.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    printf("Failed to open input file!\n");
    return 1;
  }

  char* line = NULL;
  size_t line_length = 0;
  while (getline(&line, &line_length, fp) != -1) {
    printf("%s", line);
  }

  free(line);
  fclose(fp);
  return 0;
}
