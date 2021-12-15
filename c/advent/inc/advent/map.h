#pragma once

#include <stddef.h>

#define MAP_PROTO_(MapType, ValueType)                                                                                 \
    void MapType##_init(MapType* m);                                                                                   \
    void MapType##_deinit(MapType* m);                                                                                 \
    bool MapType##_insert(MapType* m, const void* key, size_t keyLength, const ValueType value);                       \
    bool MapType##_contains(MapType* m, const void* key, size_t keyLength);                                            \
    bool MapType##_get(MapType* m, const void* key, size_t keyLength, ValueType* value);                               \
    void MapType##_clear(MapType* m)

#define DECLARE_MAP(MapType, ValueType)                                                                                \
    typedef struct                                                                                                     \
    {                                                                                                                  \
        void* key;                                                                                                     \
        size_t keyLen;                                                                                                 \
        ValueType value;                                                                                               \
    } MapType##Bucket;                                                                                                 \
    typedef struct                                                                                                     \
    {                                                                                                                  \
        MapType##Bucket* buckets;                                                                                      \
        size_t bucketCount;                                                                                            \
        size_t entryCount;                                                                                             \
    } MapType;                                                                                                         \
    MAP_PROTO_(MapType, ValueType)

#define DEFINE_MAP(MapType, ValueType)                                                                                 \
    static const double MapType##_MAX_LOAD = 0.75;                                                                     \
    static const uint64_t MapType##_FNV_PRIME = 0x100000001b3;                                                         \
    static const uint64_t MapType##_FNV_OFFSET_BASIS = 0xCBF29CE484222325;                                             \
    uint64_t MapType##_hash_(const void* key, size_t keyLen)                                                           \
    {                                                                                                                  \
        uint64_t hash = MapType##_FNV_OFFSET_BASIS;                                                                    \
        for (size_t i = 0; i < keyLen; i++)                                                                            \
        {                                                                                                              \
            hash ^= ((const uint8_t*)key)[i];                                                                          \
            hash *= MapType##_FNV_PRIME;                                                                               \
        }                                                                                                              \
        return hash;                                                                                                   \
    }                                                                                                                  \
    void MapType##_init(MapType* m)                                                                                    \
    {                                                                                                                  \
        m->buckets = NULL;                                                                                             \
        m->bucketCount = 0;                                                                                            \
        m->entryCount = 0;                                                                                             \
    }                                                                                                                  \
    void MapType##_deinit(MapType* m)                                                                                  \
    {                                                                                                                  \
        if (m->buckets != NULL)                                                                                        \
        {                                                                                                              \
            free(m->buckets);                                                                                          \
            m->buckets = NULL;                                                                                         \
            m->bucketCount = 0;                                                                                        \
            m->entryCount = 0;                                                                                         \
        }                                                                                                              \
    }                                                                                                                  \
    MapType##Bucket* MapType##_findBucket_(MapType##Bucket* buckets, size_t bucketCount, const void* key,              \
                                           size_t keyLength)                                                           \
    {                                                                                                                  \
        if (bucketCount == 0)                                                                                          \
        {                                                                                                              \
            return NULL;                                                                                               \
        }                                                                                                              \
        uint64_t h = MapType##_hash_(key, keyLength);                                                                  \
        while (buckets[h].key != NULL &&                                                                               \
               !(buckets[h].keyLen == keyLength && memcmp(buckets[h].key, key, keyLength) == 0))                       \
        {                                                                                                              \
            h = (h + 1) % bucketCount;                                                                                 \
        }                                                                                                              \
        return &buckets[h];                                                                                            \
    }                                                                                                                  \
    bool MapType##_insert(MapType* m, const void* key, size_t keyLength, const ValueType value)                        \
    {                                                                                                                  \
        if (m->buckets == NULL)                                                                                        \
        {                                                                                                              \
            m->bucketCount = 16;                                                                                       \
            m->buckets = malloc(sizeof(MapType##Bucket) * m->bucketCount);                                             \
            if (m->buckets == NULL)                                                                                    \
            {                                                                                                          \
                return false;                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        else if ((double)m->entryCount / (double)m->bucketCount > MapType##_MAX_LOAD)                                  \
        {                                                                                                              \
            MapType##Bucket* oldBuckets = m->buckets;                                                                  \
            m->bucketCount *= 2;                                                                                       \
            m->buckets = malloc(sizeof(MapType##Bucket) * m->bucketCount);                                             \
            if (m->buckets == NULL)                                                                                    \
            {                                                                                                          \
                m->buckets = oldBuckets;                                                                               \
                return false;                                                                                          \
            }                                                                                                          \
            memcpy(m->buckets, oldBuckets, sizeof(MapType##Bucket) * m->bucketCount / 2);                              \
            free(oldBuckets);                                                                                          \
        }                                                                                                              \
        MapType##Bucket* bucket = m->buckets + m->entryCount;                                                          \
        bucket->key = malloc(keyLength);                                                                               \
        if (bucket->key == NULL)                                                                                       \
        {                                                                                                              \
            return false;                                                                                              \
        }                                                                                                              \
        memcpy(bucket->key, key, keyLength);                                                                           \
        bucket->keyLen = keyLength;                                                                                    \
        bucket->value = value;                                                                                         \
        m->entryCount++;                                                                                               \
        return true;                                                                                                   \
    }                                                                                                                  \
    bool MapType##_contains(MapType* m, const void* key, size_t keyLength)                                             \
    {                                                                                                                  \
        MapType##Bucket* bucket = MapType##_findBucket_(m->buckets, m->bucketCount, key, keyLength);                   \
        return bucket != NULL && bucket->key != NULL && bucket->keyLen == keyLength &&                                 \
               memcmp(bucket->key, key, keyLength) == 0;                                                               \
    }                                                                                                                  \
    bool MapType##_get(MapType* m, const void* key, size_t keyLength, ValueType* value)                                \
    {                                                                                                                  \
        MapType##Bucket* bucket = MapType##_findBucket_(m->buckets, m->bucketCount, key, keyLength);                   \
        if (bucket != NULL && bucket->key != NULL && bucket->keyLen == keyLength &&                                    \
            memcmp(bucket->key, key, keyLength) == 0)                                                                  \
        {                                                                                                              \
            *value = bucket->value;                                                                                    \
            return true;                                                                                               \
        }                                                                                                              \
        return false;                                                                                                  \
    }                                                                                                                  \
    void MapType##_clear(MapType* m)                                                                                   \
    {                                                                                                                  \
        for (size_t i = 0; i < m->bucketCount; i++)                                                                    \
        {                                                                                                              \
            if (m->buckets[i].key != NULL)                                                                             \
            {                                                                                                          \
                free(m->buckets[i].key);                                                                               \
                m->buckets[i].key = NULL;                                                                              \
                m->buckets[i].keyLen = 0;                                                                              \
            }                                                                                                          \
        }                                                                                                              \
        m->entryCount = 0;                                                                                             \
    }
