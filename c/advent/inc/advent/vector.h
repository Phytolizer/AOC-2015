#pragma once

#include <stddef.h>

#define VECTOR_PROTO_(VectorType, ValueType)                                   \
    void VectorType##_init(VectorType* v);                                     \
    void VectorType##_deinit(VectorType* v);                                   \
    void VectorType##_push(VectorType* v, ValueType value);                    \
    VectorType VectorType##_dup(VectorType* v)

#define DECLARE_VECTOR(VectorType, ValueType)                                  \
    typedef struct {                                                           \
        ValueType* data;                                                       \
        size_t length;                                                         \
        size_t capacity;                                                       \
    } VectorType;                                                              \
    VECTOR_PROTO_(VectorType, ValueType)

#define DEFINE_VECTOR(VectorType, ValueType)                                   \
    void VectorType##_init(VectorType* v) {                                    \
        v->data = NULL;                                                        \
        v->length = 0;                                                         \
        v->capacity = 0;                                                       \
    }                                                                          \
    void VectorType##_deinit(VectorType* v) {                                  \
        free(v->data);                                                         \
        v->data = NULL;                                                        \
        v->length = 0;                                                         \
        v->capacity = 0;                                                       \
    }                                                                          \
    void VectorType##_push(VectorType* v, ValueType value) {                   \
        if (v->length == v->capacity) {                                        \
            v->capacity = v->capacity == 0 ? 1 : v->capacity * 2;              \
            v->data = realloc(v->data, v->capacity * sizeof(ValueType));       \
        }                                                                      \
        v->data[v->length++] = value;                                          \
    }                                                                          \
    VectorType VectorType##_dup(VectorType* v) {                               \
        VectorType result;                                                     \
        result.data = malloc(v->length * sizeof(ValueType));                   \
        result.length = v->length;                                             \
        result.capacity = v->capacity;                                         \
        memcpy(result.data, v->data, v->length * sizeof(ValueType));           \
        return result;                                                         \
    }
