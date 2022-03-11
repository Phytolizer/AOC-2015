#include "advent/permute.h"

#include <stdlib.h>
#include <string.h>

DEFINE_VECTOR(permutation_t, permutation_element_t);
DEFINE_VECTOR(permutation_vec_t, permutation_t);

permutation_t first_permutation(size_t n) {
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

bool next_permutation(permutation_t* p) {
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

permutation_element_t* permutation_swap(permutation_t* permutation, size_t i) {
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
