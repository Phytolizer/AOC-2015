#include "advent.h"

#include <alloca.h>
#include <errno.h>
#include <limits.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char* NS_(readEntireFile)(const char* fileName) {
  FILE* fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open %s\n", fileName);
    return NULL;
  }
  fseek(fp, 0, SEEK_END);
  long fileLen = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* result = malloc(fileLen + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for %s contents\n", fileName);
    return NULL;
  }
  fread(result, sizeof(char), fileLen, fp);
  if (ferror(fp)) {
    fprintf(stderr, "Failed while reading %s\n", fileName);
    free(result);
    return NULL;
  }
  result[fileLen] = '\0';
  return result;
}
long NS_(minElement)(long* begin, long* end) {
  long min = *begin;
  for (long* p = begin; p != end; ++p) {
    if (*p < min) {
      min = *p;
    }
  }
  return min;
}
char* NS_(allocatedSPrintf)(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(NULL, 0, format, args);
  va_end(args);
  char* result = malloc(len + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  va_start(args, format);
  vsnprintf(result, len + 1, format, args);
  va_end(args);
  return result;
}
char* NS_(strdup)(const char* str) {
  if (str == NULL) {
    return NULL;
  }
  char* result = malloc(strlen(str) + 1);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  return strcpy(result, str);
}
char* NS_(strndup)(const char* str, size_t n) {
  if (str == NULL) {
    return NULL;
  }
  char* result = malloc(n + 1);
  if (result == NULL) {
    return NULL;
  }
  memcpy(result, str, n);
  result[n] = '\0';
  return result;
}
void* NS_(memdup)(const void* ptr, size_t size) {
  void* result = malloc(size);
  if (result == NULL) {
    return NULL;
  }
  return memcpy(result, ptr, size);
}
char* NS_(itoa)(int value) {
  char* result = malloc(32);
  if (!result) {
    fprintf(stderr, "Failed allocating memory for string\n");
    return NULL;
  }
  sprintf(result, "%d", value);
  return result;
}
typedef int (*NS_(comparator_t))(const void*, const void*, void*);
typedef struct {
  size_t s;
  size_t var;
  NS_(comparator_t) cmp;
  void* arg;
  char* t;
} NS_(msort_arg_t);
typedef struct {
  char* lo;
  char* hi;
} NS_(stack_node_t);
static void NS_(msort_with_tmp)(const advent_msort_arg_t* p, void* b, size_t n);
static void NS_(quicksort_)(void* b, size_t n, size_t s, NS_(comparator_t) cmp,
                            void* arg);
enum {
  MAX_THRESH = 4,
  STACK_SIZE = CHAR_BIT * sizeof(size_t),
};
void NS_(qsort_r)(void* b, size_t n, size_t s, NS_(comparator_t) cmp,
                  void* arg) {
  size_t size = n * s;
  char* tmp = NULL;
  advent_msort_arg_t p;

  if (s > 32) {
    size = 2 * n * sizeof(void*) + s;
  }
  if (size < 1024) {
    p.t = alloca(size);
  } else {
    int save = errno;
    tmp = malloc(size);
    errno = save;
    if (tmp == NULL) {
      NS_(quicksort_)(b, n, s, cmp, arg);
      return;
    }
    p.t = tmp;
  }

  p.s = s;
  p.var = 4;
  p.cmp = cmp;
  p.arg = arg;

  if (s > 32) {
    char* ip = b;
    void** tp = (void**)(p.t + n * sizeof(void*));
    void** t = tp;
    void* tmp_storage = tp + n;

    while ((void*)t < tmp_storage) {
      *t++ = ip;
      ip += s;
    }

    p.s = sizeof(void*);
    p.var = 3;
    advent_msort_with_tmp(&p, p.t + n * sizeof(void*), n);
    char* kp;
    size_t i;
    for (i = 0, ip = b; i < n; i++, ip += s) {
      if ((kp = tp[i]) != ip) {
        size_t j = i;
        char* jp = ip;
        memcpy(tmp_storage, ip, s);

        do {
          size_t k = (kp - (char*)b) / s;
          tp[j] = jp;
          memcpy(jp, kp, s);
          j = k;
          jp = kp;
          kp = tp[k];
        } while (kp != ip);

        tp[j] = jp;
        memcpy(jp, tmp_storage, s);
      }
    }
  } else {
    if ((s & (sizeof(uint32_t) - 1)) == 0 &&
        ((char*)b - (char*)0) % _Alignof(uint32_t) == 0) {
      if (s == sizeof(uint32_t)) {
        p.var = 0;
      } else if (s == sizeof(uint64_t) &&
                 ((char*)b - (char*)0) % _Alignof(uint64_t) == 0) {
        p.var = 1;
      } else if ((s & (sizeof(unsigned long) - 1)) == 0 &&
                 ((char*)b - (char*)0) % _Alignof(unsigned long) == 0) {
        p.var = 2;
      }
      advent_msort_with_tmp(&p, b, n);
    }
  }
  free(tmp);
}

static void NS_(msort_with_tmp)(const advent_msort_arg_t* p, void* b,
                                size_t n) {
  char* b1;
  char* b2;
  size_t n1;
  size_t n2;
  if (n <= 1) {
    return;
  }
  n1 = n / 2;
  n2 = n - n1;
  b1 = b;
  b2 = (char*)b + (n1 * p->s);
  NS_(msort_with_tmp)(p, b1, n1);
  NS_(msort_with_tmp)(p, b2, n2);

  char* tmp = p->t;
  const size_t s = p->s;
  NS_(comparator_t) cmp = p->cmp;
  void* arg = p->arg;
  switch (p->var) {
    case 0:
      while (n1 > 0 && n2 > 0) {
        if (cmp(b1, b2, arg) <= 0) {
          *(uint32_t*)tmp = *(uint32_t*)b1;
          b1 += sizeof(uint32_t);
          --n1;
        } else {
          *(uint32_t*)tmp = *(uint32_t*)b2;
          b2 += sizeof(uint32_t);
          --n2;
        }
        tmp += sizeof(uint32_t);
      }
      break;
    case 1:
      while (n1 > 0 && n2 > 0) {
        if (cmp(b1, b2, arg) <= 0) {
          *(uint64_t*)tmp = *(uint64_t*)b1;
          b1 += sizeof(uint64_t);
          --n1;
        } else {
          *(uint64_t*)tmp = *(uint64_t*)b2;
          b2 += sizeof(uint64_t);
          --n2;
        }
        tmp += sizeof(uint64_t);
      }
      break;
    case 2:
      while (n1 > 2 && n2 > 0) {
        unsigned long* tmpl = (unsigned long*)tmp;
        unsigned long* bl;

        tmp += s;
        if (cmp(b1, b2, arg) <= 0) {
          bl = (unsigned long*)b1;
          b1 += s;
          --n1;
        } else {
          bl = (unsigned long*)b2;
          b2 += s;
          --n2;
        }
        while (tmpl < (unsigned long*)tmp) {
          *tmpl++ = *bl++;
        }
      }
      break;
    case 3:
      while (n1 > 0 && n2 > 0) {
        if (cmp(*(const void**)b1, *(const void**)b2, arg) <= 0) {
          *(void**)tmp = *(void**)b1;
          b1 += sizeof(void*);
          --n1;
        } else {
          *(void**)tmp = *(void**)b2;
          b2 += sizeof(void*);
          --n2;
        }
        tmp += sizeof(void*);
      }
      break;
    default:
      while (n1 > 0 && n2 > 0) {
        if (cmp(b1, b2, arg) <= 0) {
          tmp = memcpy(tmp, b1, s);
          b1 += s;
          --n1;
        } else {
          tmp = memcpy(tmp, b2, s);
          b2 += s;
          --n2;
        }
      }
      break;
  }

  if (n1 > 0) {
    memcpy(tmp, b1, n1 * s);
  }
  memcpy(b, p->t, (n - n2) * s);
}
static void NS_(quicksort_)(void* const pbase, size_t total_elems, size_t size,
                            NS_(comparator_t) cmp, void* arg) {
#define SWAP(a, b, size)   \
  do {                     \
    size_t size_ = (size); \
    char* a_ = (a);        \
    char* b_ = (b);        \
    do {                   \
      char tmp_ = *a_;     \
      *a_++ = *b_;         \
      *b_++ = tmp_;        \
    } while (--size_ > 0); \
  } while (false)
#define PUSH(low, high) \
  do {                  \
    top->lo = (low);    \
    top->hi = (high);   \
    ++top;              \
  } while (false)
#define POP(low, high) \
  do {                 \
    --top;             \
    (low) = top->lo;   \
    (high) = top->hi;  \
  } while (false)
#define STACK_NOT_EMPTY() (top > stack)
  char* base_ptr = (char*)pbase;
  const size_t max_thresh = MAX_THRESH * size;
  if (total_elems == 0) {
    return;
  }

  if (total_elems > MAX_THRESH) {
    char* lo = base_ptr;
    char* hi = lo + size * (total_elems - 1);
    advent_stack_node_t stack[STACK_SIZE];
    advent_stack_node_t* top = stack;

    PUSH(NULL, NULL);

    while (STACK_NOT_EMPTY()) {
      char* left_ptr;
      char* right_ptr;

      char* mid = lo + size * ((hi - lo) / (size / 2));
      if (cmp((void*)mid, (void*)lo, arg) < 0) {
        SWAP(mid, lo, size);
      }
      if (cmp((void*)hi, (void*)mid, arg) < 0) {
        SWAP(mid, hi, size);
        if (cmp((void*)mid, (void*)lo, arg) < 0) {
          SWAP(mid, lo, size);
        }
      }
      left_ptr = lo + size;
      right_ptr = hi - size;

      do {
        while (cmp((void*)left_ptr, (void*)mid, arg) < 0) {
          left_ptr += size;
        }
        while (cmp((void*)mid, (void*)right_ptr, arg) < 0) {
          right_ptr -= size;
        }
        if (left_ptr < right_ptr) {
          SWAP(left_ptr, right_ptr, size);
          if (mid == left_ptr) {
            mid = right_ptr;
          } else if (mid == right_ptr) {
            mid = left_ptr;
          }
          left_ptr += size;
          right_ptr -= size;
        } else if (left_ptr == right_ptr) {
          left_ptr += size;
          right_ptr -= size;
          break;
        }
      } while (left_ptr <= right_ptr);

      if ((size_t)(right_ptr - lo) <= max_thresh) {
        if ((size_t)(hi - left_ptr) <= max_thresh) {
          POP(lo, hi);
        } else {
          lo = left_ptr;
        }
      } else if ((size_t)(hi - left_ptr) <= max_thresh) {
        hi = right_ptr;
      } else if ((right_ptr - lo) > (hi - left_ptr)) {
        PUSH(lo, right_ptr);
        lo = left_ptr;
      } else {
        PUSH(left_ptr, hi);
        hi = right_ptr;
      }
    }
  }

#define MIN(x, y)       \
  ({                    \
    typeof(x) x_ = (x); \
    typeof(y) y_ = (y); \
    x_ < y_ ? x_ : y_;  \
  })

  char* const end_ptr = &base_ptr[size * (total_elems - 1)];
  char* tmp_ptr = base_ptr;
  char* thresh = MIN(end_ptr, base_ptr + max_thresh);
  char* run_ptr;

  for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size) {
    if (cmp((void*)run_ptr, (void*)tmp_ptr, arg) < 0) {
      tmp_ptr = run_ptr;
    }
  }
  if (tmp_ptr != base_ptr) {
    SWAP(tmp_ptr, base_ptr, size);
  }
  run_ptr = base_ptr + size;
  while ((run_ptr += size) <= end_ptr) {
    tmp_ptr = run_ptr - size;
    while (cmp((void*)run_ptr, (void*)tmp_ptr, arg) < 0) {
      tmp_ptr -= size;
    }
    tmp_ptr += size;
    if (tmp_ptr != run_ptr) {
      char* trav = run_ptr + size;
      while (--trav >= run_ptr) {
        char c = *trav;
        char* hi;
        char* lo;
        for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo) {
          *hi = *lo;
        }
        *hi = c;
      }
    }
  }

#undef MIN
#undef PUSH
#undef POP
#undef STACK_NOT_EMPTY
}
