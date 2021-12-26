#include <advent.h>
#include <advent/map.h>
#include <advent/pcre.h>
#include <advent/vector.h>
#include <assert.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

DECLARE_VECTOR(StringVector, char*);
DEFINE_VECTOR(StringVector, char*);

#define DEBUG_OUTPUT true

typedef uint16_t SignalStrength;

DECLARE_MAP(SignalStrengthMap, SignalStrength);
DEFINE_MAP(SignalStrengthMap, SignalStrength);

#define OPS_ \
  X(SET)     \
  X(NOT)     \
  X(RSHIFT)  \
  X(LSHIFT)  \
  X(AND)     \
  X(OR)

typedef enum {
#define X(x) OP_##x,
  OPS_
#undef X
} Op;

const char* opStr(Op op) {
  switch (op) {
#define X(x)   \
  case OP_##x: \
    return #x;
    OPS_
#undef X
        default : fprintf(stderr, "Corrupt op: %d\n", op);
    exit(EXIT_FAILURE);
  }
}

typedef struct {
  char* name;
  Op op;
  char* inputA;
  char* inputB;
  char* description;
  SignalStrength* value1;
  SignalStrength* bits;
} Gate;

DECLARE_MAP(GateMap, Gate);
DEFINE_MAP(GateMap, Gate);

void Gate_init(Gate* g, const char* name, Op op, const char* inputA,
               const char* inputB, pcre2_code* wirePattern) {
  memset(g, 0, sizeof(Gate));
  g->name = advent_strdup(name);
  g->inputA = advent_strdup(inputA);
  g->inputB = advent_strdup(inputB);
  if (inputA == NULL) {
    g->description = advent_allocatedSPrintf("%s %s", opStr(op), inputB);
  } else if (inputB == NULL) {
    g->description = advent_allocatedSPrintf("%s", inputA);
  } else {
    g->description =
        advent_allocatedSPrintf("%s %s %s", inputA, opStr(op), inputB);
  }
  pcre2_match_data* wireMatchData =
      pcre2_match_data_create_from_pattern(wirePattern, NULL);

  switch (op) {
    case OP_SET: {
      int rc = pcre2_match(wirePattern, (PCRE2_SPTR)inputA,
                           PCRE2_ZERO_TERMINATED, 0, 0, wireMatchData, 0);
      if (rc == PCRE2_ERROR_NOMATCH) {
        g->value1 = malloc(sizeof(SignalStrength));
        *g->value1 = strtol(inputA, NULL, 10);
      }
    } break;
    case OP_LSHIFT:
    case OP_RSHIFT:
      assert(pcre2_match(wirePattern, (PCRE2_SPTR)inputA, PCRE2_ZERO_TERMINATED,
                         0, 0, wireMatchData, 0) != PCRE2_ERROR_NOMATCH &&
             "Invalid SHIFT gate");
      g->bits = malloc(sizeof(SignalStrength));
      *g->bits = strtol(inputB, NULL, 10);
      break;
    case OP_NOT:
      assert(inputA == NULL &&
             pcre2_match(wirePattern, (PCRE2_SPTR)inputB, PCRE2_ZERO_TERMINATED,
                         0, 0, wireMatchData, 0) != PCRE2_ERROR_NOMATCH &&
             "Invalid NOT gate");
      break;
    case OP_AND:
    case OP_OR:
      assert(pcre2_match(wirePattern, (PCRE2_SPTR)inputB, PCRE2_ZERO_TERMINATED,
                         0, 0, wireMatchData, 0) != PCRE2_ERROR_NOMATCH &&
             "Invalid AND/OR gate");
      if (pcre2_match(wirePattern, (PCRE2_SPTR)inputA, PCRE2_ZERO_TERMINATED, 0,
                      0, wireMatchData, 0) == PCRE2_ERROR_NOMATCH) {
        g->value1 = malloc(sizeof(SignalStrength));
        *g->value1 = strtol(inputA, NULL, 10);
      }
      break;
  }
  pcre2_match_data_free(wireMatchData);
}

void Gate_deinit(Gate* g) {
  free(g->name);
  free(g->description);
  free(g->inputA);
  free(g->inputB);
  free(g->value1);
  free(g->bits);
}

SignalStrength Gate_eval(Gate* g, GateMap* gates, SignalStrengthMap* cache,
                         int depth) {
#if DEBUG_OUTPUT
  printf("%*s%s => %s\n", depth * 2, "", g->name, g->description);
#endif
  SignalStrength* cached_result;
  if (SignalStrengthMap_get(cache, g->name, strlen(g->name), &cached_result)) {
#if DEBUG_OUTPUT
    printf("%*s=> %d (cache hit)\n", depth * 2, "", *cached_result);
#endif
    return *cached_result;
  }
  SignalStrength result;
  switch (g->op) {
    case OP_SET:
      if (g->value1 != NULL) {
        return *g->value1;
      } else {
        Gate* a;
        GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
        result = Gate_eval(a, gates, cache, depth + 1);
        SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
        printf("%*s=> %d (set)\n", depth * 2, "", result);
#endif
        return result;
      }
    case OP_NOT: {
      Gate* a;
      GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
      result = ~Gate_eval(a, gates, cache, depth + 1);
      SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
      printf("%*s=> %d (NOT)\n", depth * 2, "", result);
#endif
      return result;
    }
    case OP_RSHIFT: {
      Gate* a;
      GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
      result = Gate_eval(a, gates, cache, depth + 1) >> *g->bits;
      SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
      printf("%*s=> %d (RSHIFT)\n", depth * 2, "", result);
#endif
      return result;
    }
    case OP_LSHIFT: {
      Gate* a;
      GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
      result = Gate_eval(a, gates, cache, depth + 1) << *g->bits;
      SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
      printf("%*s=> %d (LSHIFT)\n", depth * 2, "", result);
#endif
      return result;
    }
    case OP_AND:
      if (g->value1 != NULL) {
        Gate* b;
        GateMap_get(gates, g->inputB, strlen(g->inputB), &b);
        result = Gate_eval(b, gates, cache, depth + 1) & *g->value1;
#if DEBUG_OUTPUT
        printf("%*s=> %d (AND)\n", depth * 2, "", result);
#endif
        return result;
      } else {
        Gate* a;
        GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
        Gate* b;
        GateMap_get(gates, g->inputB, strlen(g->inputB), &b);
        result = Gate_eval(a, gates, cache, depth + 1) &
                 Gate_eval(b, gates, cache, depth + 1);
        SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
        printf("%*s=> %d (AND)\n", depth * 2, "", result);
#endif
        return result;
      }
    case OP_OR:
      if (g->value1 != NULL) {
        Gate* b;
        GateMap_get(gates, g->inputB, strlen(g->inputB), &b);
        result = Gate_eval(b, gates, cache, depth + 1) | *g->value1;
#if DEBUG_OUTPUT
        printf("%*s=> %d (OR)\n", depth * 2, "", result);
#endif
        return result;
      } else {
        Gate* a;
        GateMap_get(gates, g->inputA, strlen(g->inputA), &a);
        Gate* b;
        GateMap_get(gates, g->inputB, strlen(g->inputB), &b);
        result = Gate_eval(a, gates, cache, depth + 1) |
                 Gate_eval(b, gates, cache, depth + 1);
        SignalStrengthMap_insert(cache, g->name, strlen(g->name), result);
#if DEBUG_OUTPUT
        printf("%*s=> %d (OR)\n", depth * 2, "", result);
#endif
        return result;
      }
  }
}

bool parseLine(const char* line, pcre2_code* linePattern,
               pcre2_match_data* lineMatchData, pcre2_code* wirePattern,
               Gate* result) {
  int rc = pcre2_match(linePattern, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED, 0,
                       0, lineMatchData, 0);
  if (rc < 0) {
    return false;
  } else if (rc == 0) {
    return false;
  } else {
    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(lineMatchData);
    char* name = malloc(ovector[5] - ovector[4] + 1);
    memcpy(name, line + ovector[4], ovector[5] - ovector[4]);
    name[ovector[5] - ovector[4]] = '\0';
    char* inputs = malloc(ovector[3] - ovector[2] + 1);
    memcpy(inputs, &line[ovector[2]], ovector[3] - ovector[2]);
    inputs[ovector[3] - ovector[2]] = '\0';
    StringVector words;
    StringVector_init(&words);
    for (char* word = strtok(inputs, " "); word != NULL;
         word = strtok(NULL, " ")) {
      StringVector_push(&words, word);
    }
    switch (words.length) {
      case 1:
        Gate_init(result, name, OP_SET, words.data[0], NULL, wirePattern);
        break;
      case 2:
        if (strcmp(words.data[0], "NOT") == 0) {
          Gate_init(result, name, OP_NOT, NULL, words.data[1], wirePattern);
        } else if (strcmp(words.data[0], "RSHIFT") == 0) {
          Gate_init(result, name, OP_RSHIFT, NULL, words.data[1], wirePattern);
        } else if (strcmp(words.data[0], "LSHIFT") == 0) {
          Gate_init(result, name, OP_LSHIFT, NULL, words.data[1], wirePattern);
        } else if (strcmp(words.data[0], "AND") == 0) {
          Gate_init(result, name, OP_AND, NULL, words.data[1], wirePattern);
        } else if (strcmp(words.data[0], "OR") == 0) {
          Gate_init(result, name, OP_OR, NULL, words.data[1], wirePattern);
        } else {
          return false;
        }
        break;
      case 3:
        if (strcmp(words.data[1], "NOT") == 0) {
          Gate_init(result, name, OP_NOT, words.data[0], words.data[2],
                    wirePattern);
        } else if (strcmp(words.data[1], "RSHIFT") == 0) {
          Gate_init(result, name, OP_RSHIFT, words.data[0], words.data[2],
                    wirePattern);
        } else if (strcmp(words.data[1], "LSHIFT") == 0) {
          Gate_init(result, name, OP_LSHIFT, words.data[0], words.data[2],
                    wirePattern);
        } else if (strcmp(words.data[1], "AND") == 0) {
          Gate_init(result, name, OP_AND, words.data[0], words.data[2],
                    wirePattern);
        } else if (strcmp(words.data[1], "OR") == 0) {
          Gate_init(result, name, OP_OR, words.data[0], words.data[2],
                    wirePattern);
        } else {
          return false;
        }
        break;
      default:
        return false;
    }
    StringVector_deinit(&words);
    free(name);
    free(inputs);
    return true;
  }
}

int main(void) {
  int errCode;
  size_t errOfs;
  pcre2_code* wirePattern =
      pcre2_compile((PCRE2_SPTR) "^[a-z]+$", PCRE2_ZERO_TERMINATED, 0, &errCode,
                    &errOfs, NULL);
  if (errCode < 0) {
    char message[120];
    pcre2_get_error_message(errCode, (PCRE2_UCHAR*)message, 120);
    fprintf(stderr, "PCRE compile error: %s\n", message);
    return 1;
  }
  pcre2_code* linePattern =
      pcre2_compile((PCRE2_SPTR) "(.+) -> (\\S+)", PCRE2_ZERO_TERMINATED, 0,
                    &errCode, &errOfs, NULL);
  if (errCode < 0) {
    char message[120];
    pcre2_get_error_message(errCode, (PCRE2_UCHAR*)message, 120);
    fprintf(stderr, "PCRE compile error: %s\n", message);
    return 1;
  }
  pcre2_match_data* lineMatchData =
      pcre2_match_data_create_from_pattern(linePattern, NULL);

  GateMap gates;
  GateMap_init(&gates);

  FILE* input = fopen(ADVENT_INPUT, "r");
  if (input == NULL) {
    fprintf(stderr, "Failed to open input file: %s\n", ADVENT_INPUT);
    return 1;
  }

  char* line = NULL;
  size_t lineLen = 0;
  while (getline(&line, &lineLen, input) != -1) {
    Gate g;
    if (parseLine(line, linePattern, lineMatchData, wirePattern, &g)) {
      assert(GateMap_insert(&gates, g.name, strlen(g.name), g));
    }
    free(line);
    line = NULL;
  }
  free(line);

  SignalStrengthMap cache;
  SignalStrengthMap_init(&cache);
  Gate* a;
  assert(GateMap_get(&gates, "a", 1, &a));
  SignalStrength aStrength = Gate_eval(a, &gates, &cache, 0);
  printf("a: %d\n", aStrength);
  Gate* b;
  GateMap_get(&gates, "b", 1, &b);
  Gate_deinit(b);
  char* aStrengthStr = advent_itoa(aStrength);
  Gate_init(b, "b", OP_SET, aStrengthStr, NULL, wirePattern);
  free(aStrengthStr);
  assert(GateMap_insert(&gates, "b", 1, *b));
  SignalStrengthMap_clear(&cache);
  aStrength = Gate_eval(a, &gates, &cache, 0);
  printf("a: %d\n", aStrength);

  for (size_t i = 0; i < cache.bucketCount; ++i) {
    if (cache.buckets[i].key != NULL) {
      free(cache.buckets[i].key);
    }
  }
  SignalStrengthMap_deinit(&cache);
  for (size_t i = 0; i < gates.bucketCount; ++i) {
    if (gates.buckets[i].key != NULL) {
      free(gates.buckets[i].key);
      Gate_deinit(&gates.buckets[i].value);
    }
  }
  GateMap_deinit(&gates);
  pcre2_match_data_free(lineMatchData);
  pcre2_code_free(linePattern);
  pcre2_code_free(wirePattern);
  return 0;
}
