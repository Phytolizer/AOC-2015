#include <advent.h>
#include <advent/map.h>
#include <advent/pcre.h>
#include <advent/set.h>
#include <advent/vector.h>
#include <assert.h>
#include <config.h>
#include <pcre2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* a;
  char* b;
  size_t dist;
} edge_t;

void edge_deinit(edge_t* e) {
  free(e->a);
  free(e->b);
}

DECLARE_VECTOR(vec_str_t, char*);
DEFINE_VECTOR(vec_str_t, char*);

typedef struct {
  size_t distance;
  vec_str_t path;
} path_t;

typedef struct {
  size_t index;
  int direction;
} permutation_element_t;

DECLARE_VECTOR(edge_vec_t, edge_t);
DEFINE_VECTOR(edge_vec_t, edge_t);
DECLARE_VECTOR(path_vec_t, path_t);
DEFINE_VECTOR(path_vec_t, path_t);
DECLARE_VECTOR(permutation_t, permutation_element_t);
DEFINE_VECTOR(permutation_t, permutation_element_t);
DECLARE_VECTOR(permutation_vec_t, permutation_t);
DEFINE_VECTOR(permutation_vec_t, permutation_t);
DECLARE_VECTOR(str_vec_t, char*);
DEFINE_VECTOR(str_vec_t, char*);

DECLARE_MAP(size_map_t, size_t);
DEFINE_MAP(size_map_t, size_t);
DECLARE_MAP(graph_t, size_map_t);
DEFINE_MAP(graph_t, size_map_t);

static permutation_t first_permutation(size_t n);
static permutation_vec_t all_permutations(size_t n);
static bool next_permutation(permutation_t* p);
static permutation_element_t* permutation_swap(permutation_t* permutation,
                                               size_t i);

int main(void) {
  int error;
  size_t errofs;
  pcre2_code* code =
      pcre2_compile((PCRE2_SPTR) "(\\w+) to (\\w+) = (\\d+)",
                    PCRE2_ZERO_TERMINATED, 0, &error, &errofs, NULL);
  if (code == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(error, buffer, sizeof(buffer));
    fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)errofs,
            buffer);
    return 1;
  }
  pcre2_match_data* match_data =
      pcre2_match_data_create_from_pattern(code, NULL);
  if (match_data == NULL) {
    fprintf(stderr, "Failed to create match data\n");
    return 1;
  }

  FILE* fp = fopen(ADVENT_INPUT, "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open input file: %s\n", ADVENT_INPUT);
    return 1;
  }

  Set nodes;
  Set_init(&nodes);
  edge_vec_t edges;
  edge_vec_t_init(&edges);

  char* line = NULL;
  size_t line_length = 0;
  while (getline(&line, &line_length, fp) != -1) {
    line[strlen(line) - 1] = '\0';
    int ret = pcre2_match(code, (PCRE2_SPTR)line, PCRE2_ZERO_TERMINATED, 0, 0,
                          match_data, NULL);
    if (ret < 0) {
      fprintf(stderr, "PCRE2 match failed: %d\n", ret);
      return 1;
    }
    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
    char* from = line + ovector[2];
    char* to = line + ovector[4];
    char* distance = line + ovector[6];
    Set_insert(&nodes, from, ovector[3] - ovector[2]);
    Set_insert(&nodes, to, ovector[5] - ovector[4]);
    edge_t edge = {
        .a = advent_strndup(from, ovector[3] - ovector[2]),
        .b = advent_strndup(to, ovector[5] - ovector[4]),
        .dist = strtol(distance, NULL, 10),
    };
    edge_vec_t_push(&edges, edge);
    free(line);
    line = NULL;
  }

  graph_t graph;
  graph_t_init(&graph);
  for (size_t i = 0; i < nodes.capacity; ++i) {
    if (nodes.data[i].value != NULL) {
      size_map_t map;
      size_map_t_init(&map);
      graph_t_insert(&graph, nodes.data[i].value, nodes.data[i].valueLength,
                     map);
    }
  }

  for (size_t i = 0; i < edges.length; ++i) {
    edge_t* e = &edges.data[i];
    size_map_t* from;
    assert(graph_t_get(&graph, e->a, strlen(e->a), &from) && "faulty logic");
    size_map_t_insert(from, e->b, strlen(e->b), e->dist);
    size_map_t* to;
    assert(graph_t_get(&graph, e->b, strlen(e->b), &to) && "faulty logic");
    size_map_t_insert(to, e->a, strlen(e->a), e->dist);
  }

  path_vec_t paths;
  path_vec_t_init(&paths);
  str_vec_t straight_nodes;
  str_vec_t_init(&straight_nodes);
  for (size_t i = 0; i < nodes.capacity; ++i) {
    SetBucket* bucket = &nodes.data[i];
    if (bucket->value != NULL) {
      str_vec_t_push(&straight_nodes,
                     advent_strndup(bucket->value, bucket->valueLength));
    }
  }
  size_t min_distance = SIZE_MAX;
  size_t max_distance = 0;
  permutation_t permutation = first_permutation(straight_nodes.length);
  do {
    size_t distance = 0;
    for (size_t i = 0; i < permutation.length - 1; ++i) {
      char* pair[2] = {straight_nodes.data[permutation.data[i].index],
                       straight_nodes.data[permutation.data[i + 1].index]};
      size_map_t* from;
      assert(graph_t_get(&graph, pair[0], strlen(pair[0]), &from) &&
             "faulty logic");
      size_t* dist;
      assert(size_map_t_get(from, pair[1], strlen(pair[1]), &dist) &&
             "faulty logic");
      distance += *dist;
    }
    if (distance < min_distance) {
      min_distance = distance;
      for (size_t i = 0; i < permutation.length; ++i) {
        printf("%s%s", i > 0 ? " -> " : "",
               straight_nodes.data[permutation.data[i].index]);
      }
      printf(": ");
      printf("%zu\n", distance);
    }
    if (distance > max_distance) {
      max_distance = distance;
    }
  } while (next_permutation(&permutation));

  printf("Part 1: %zu\n", min_distance);
  printf("Part 2: %zu\n", max_distance);

  for (size_t i = 0; i < graph.bucketCount; ++i) {
    if (graph.buckets[i].key != NULL) {
      size_map_t* map = &graph.buckets[i].value;
      size_map_t_deinit(map);
    }
  }
  graph_t_deinit(&graph);
  for (size_t i = 0; i < edges.length; ++i) {
    edge_deinit(&edges.data[i]);
  }
  edge_vec_t_deinit(&edges);
  free(line);
  pcre2_code_free(code);
  return 0;
}

static permutation_t first_permutation(size_t n) {
  permutation_t p;
  permutation_t_init(&p);
  permutation_element_t first = {0};
  permutation_t_push(&p, first);
  for (size_t i = 1; i < n; ++i) {
    permutation_element_t e = {
        .index = i,
        .direction = -1,
    };
    permutation_t_push(&p, e);
  }
  return p;
}

static permutation_vec_t all_permutations(size_t n) {
  /// Even's speedup to the Steinhaus-Johnson-Trotter algorithm
  permutation_vec_t pv;
  permutation_vec_t_init(&pv);
  permutation_t p = first_permutation(n);
  permutation_vec_t_push(&pv, p);
  size_t chosen = 0;
  while (true) {
    bool found = false;
    size_t max_index = 0;
    for (size_t i = 0; i < p.length; ++i) {
      if (p.data[i].direction != 0) {
        found = true;
        if (p.data[i].index > max_index) {
          max_index = p.data[i].index;
          chosen = i;
        }
      }
    }
    if (!found) {
      break;
    }
    permutation_t next = permutation_t_dup(&p);
    permutation_element_t* swapped = permutation_swap(&next, chosen);
    for (size_t i = 0; i < next.length; ++i) {
      if (next.data[i].index > swapped->index) {
        next.data[i].direction = i < chosen ? 1 : -1;
      }
    }
    permutation_vec_t_push(&pv, next);
    p = next;
  }
  return pv;
}

static bool next_permutation(permutation_t* p) {
  bool found = false;
  size_t max_index = 0;
  size_t chosen = 0;
  for (size_t i = 0; i < p->length; ++i) {
    if (p->data[i].direction != 0) {
      found = true;
      if (p->data[i].index > max_index) {
        max_index = p->data[i].index;
        chosen = i;
      }
    }
  }
  if (!found) {
    return false;
  }
  permutation_element_t* swapped = permutation_swap(p, chosen);
  for (size_t i = 0; i < p->length; ++i) {
    if (p->data[i].index > swapped->index) {
      p->data[i].direction = i < chosen ? 1 : -1;
    }
  }
  return true;
}

static permutation_element_t* permutation_swap(permutation_t* permutation,
                                               size_t i) {
  /// Swap the ith element with the element in its indicated direction
  permutation_element_t* e = &permutation->data[i];
  size_t next_index = i + e->direction;
  permutation_element_t* e2 = &permutation->data[next_index];
  permutation_element_t tmp = *e;
  *e = *e2;
  *e2 = tmp;
  size_t next_next_index = next_index + e2->direction;
  if (next_index == 0 || next_index == permutation->length - 1 ||
      permutation->data[next_next_index].index > e2->index) {
    e2->direction = 0;
  }
  return e2;
}
