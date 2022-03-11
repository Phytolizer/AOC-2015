#include "advent/string.h"

#include <stdlib.h>
#include <string.h>

string_t string_from_cstr(char* s) {
    return (string_t){
        .s = s,
        .l = strlen(s),
    };
}

void string_free(string_t s) {
    free(s.s);
}
