#include <advent.h>
#include <advent/map.h>
#include <advent/pcre.h>
#include <advent/set.h>
#include <advent/vector.h>
#include <assert.h>
#include <config.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "advent/permute.h"

typedef struct {
    char* s;
    size_t l;
} string_t;

string_t string_from_cstr(char* s) {
    return (string_t){
        .s = s,
        .l = strlen(s),
    };
}

void string_free(string_t s) {
    free(s.s);
}

DECLARE_MAP(happiness_catalog_entry_t, long);
DEFINE_MAP(happiness_catalog_entry_t, long);
DECLARE_MAP(happiness_catalog_t, happiness_catalog_entry_t);
DEFINE_MAP(happiness_catalog_t, happiness_catalog_entry_t);

typedef struct {
    string_t a;
    string_t b;
    long happiness;
} happiness_catalog_pair_t;

DECLARE_VECTOR(happiness_catalog_pair_vec_t, happiness_catalog_pair_t);
DEFINE_VECTOR(happiness_catalog_pair_vec_t, happiness_catalog_pair_t);

DECLARE_VECTOR(string_vec_t, string_t);
DEFINE_VECTOR(string_vec_t, string_t);

int main(void) {
    FILE* fp = fopen(ADVENT_INPUT, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s\n", ADVENT_INPUT);
        return 1;
    }

    int err;
    size_t errofs;
    pcre2_code* code =
        pcre2_compile((PCRE2_SPTR) "(\\w+) would (gain|lose) (\\d+) happiness "
                                   "units by sitting next to (\\w+)\\.",
                      PCRE2_ZERO_TERMINATED, 0, &err, &errofs, NULL);
    if (code == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(err, buffer, sizeof(buffer));
        fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n",
                (int)errofs, buffer);
        return 1;
    }

    pcre2_match_data* match_data =
        pcre2_match_data_create_from_pattern(code, NULL);
    if (match_data == NULL) {
        fprintf(stderr, "Failed to create match data\n");
        return 1;
    }

    char* line = NULL;
    size_t line_length = 0;
    Set names;
    Set_init(&names);
    happiness_catalog_pair_vec_t pairs;
    happiness_catalog_pair_vec_t_init(&pairs);
    while (getline(&line, &line_length, fp) != -1) {
        line_length = strlen(line);
        if (line[line_length - 1] == '\n') {
            --line_length;
            line[line_length] = '\0';
        }

        int ret = pcre2_match(code, (PCRE2_SPTR)line, line_length, 0, 0,
                              match_data, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to match line: %s\n", line);
            return 1;
        }

        PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
        Set_insert(&names, line + ovector[2], ovector[3] - ovector[2]);
        long happiness = strtol(line + ovector[6], NULL, 10);
        if (strncmp(line + ovector[4], "lose", ovector[5] - ovector[4]) == 0) {
            happiness = -happiness;
        }
        happiness_catalog_pair_t pair = {
            .a = string_from_cstr(
                advent_strndup(line + ovector[2], ovector[3] - ovector[2])),
            .b = string_from_cstr(
                advent_strndup(line + ovector[8], ovector[9] - ovector[8])),
            .happiness = happiness,
        };
        happiness_catalog_pair_vec_t_push(&pairs, pair);

        free(line);
        line = NULL;
    }

    happiness_catalog_t catalog;
    happiness_catalog_t_init(&catalog);
    for (size_t i = 0; i < names.capacity; ++i) {
        if (names.data[i].value == NULL) {
            continue;
        }

        happiness_catalog_entry_t entry;
        happiness_catalog_entry_t_init(&entry);
        happiness_catalog_t_insert(&catalog, names.data[i].value,
                                   names.data[i].valueLength, entry);
    }

    for (size_t i = 0; i < pairs.length; ++i) {
        happiness_catalog_entry_t* entry;
        assert(happiness_catalog_t_get(&catalog, pairs.data[i].a.s,
                                       pairs.data[i].a.l, &entry) &&
               "faulty logic");
        happiness_catalog_entry_t_insert(entry, pairs.data[i].b.s,
                                         pairs.data[i].b.l,
                                         pairs.data[i].happiness);
    }

    string_vec_t names_vec;
    string_vec_t_init(&names_vec);
    for (size_t i = 0; i < names.capacity; ++i) {
        if (names.data[i].value == NULL) {
            continue;
        }

        string_vec_t_push(&names_vec,
                          string_from_cstr(advent_strndup(
                              names.data[i].value, names.data[i].valueLength)));
    }

    happiness_catalog_entry_t entry;
    happiness_catalog_entry_t_init(&entry);
    for (size_t i = 0; i < names_vec.length; ++i) {
        happiness_catalog_entry_t_insert(&entry, names_vec.data[i].s,
                                         names_vec.data[i].l, 0);
    }
    for (size_t i = 0; i < catalog.bucketCount; ++i) {
        if (catalog.buckets[i].key == NULL) {
            continue;
        }

        happiness_catalog_entry_t* entry;
        assert(happiness_catalog_t_get(&catalog, catalog.buckets[i].key,
                                       catalog.buckets[i].keyLen, &entry) &&
               "faulty logic");
        happiness_catalog_entry_t_insert(entry, "Me", 2, 0);
    }
    happiness_catalog_t_insert(&catalog, "Me", 2, entry);
    string_vec_t_push(&names_vec, string_from_cstr(advent_strdup("Me")));

    permutation_t perm = first_permutation(names_vec.length);
    permutation_t optimal_permutation = perm;
    long optimal_happiness = 0;
    bool first = true;
    do {
        long total_happiness = 0;
        for (size_t i = 0; i < perm.length; ++i) {
            string_t name = names_vec.data[perm.data[i].index];
            happiness_catalog_entry_t* entry;
            assert(happiness_catalog_t_get(&catalog, name.s, name.l, &entry) &&
                   "faulty logic");
            string_t name2;
            if (i == 0) {
                name2 = names_vec.data[perm.data[perm.length - 1].index];
            } else {
                name2 = names_vec.data[perm.data[i - 1].index];
            }
            long* happiness;
            assert(happiness_catalog_entry_t_get(entry, name2.s, name2.l,
                                                 &happiness) &&
                   "faulty logic");
            total_happiness += *happiness;
            if (i == perm.length - 1) {
                name2 = names_vec.data[perm.data[0].index];
            } else {
                name2 = names_vec.data[perm.data[i + 1].index];
            }
            assert(happiness_catalog_entry_t_get(entry, name2.s, name2.l,
                                                 &happiness) &&
                   "faulty logic");
            total_happiness += *happiness;
        }
        if (first || total_happiness > optimal_happiness) {
            first = false;
            optimal_happiness = total_happiness;
            optimal_permutation = perm;
        }
    } while (next_permutation(&perm));
    for (size_t i = 0; i < optimal_permutation.length; ++i) {
        string_t name = names_vec.data[optimal_permutation.data[i].index];
        printf("%s%s", i == 0 ? "" : " - ", name.s);
    }
    printf(" (happiness : %ld)\n", optimal_happiness);

    for (size_t i = 0; i < names_vec.length; ++i) {
        string_free(names_vec.data[i]);
    }
    string_vec_t_deinit(&names_vec);
    free(line);
    for (size_t i = 0; i < catalog.bucketCount; ++i) {
        if (catalog.buckets[i].key == NULL) {
            continue;
        }

        happiness_catalog_entry_t_deinit(&catalog.buckets[i].value);
    }
    happiness_catalog_t_deinit(&catalog);
    for (size_t i = 0; i < pairs.length; ++i) {
        string_free(pairs.data[i].a);
        string_free(pairs.data[i].b);
    }
    happiness_catalog_pair_vec_t_deinit(&pairs);
    fclose(fp);
    pcre2_match_data_free(match_data);
    pcre2_code_free(code);
    return 0;
}