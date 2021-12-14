#include <advent.h>
#include <advent/pcre.h>
#include <stdbool.h>
#include <stdio.h>

#define DEBUG_OUTPUT false

typedef uint16_t SignalStrength;

#define OPS_                                                                                                           \
    X(SET)                                                                                                             \
    X(NOT)                                                                                                             \
    X(RSHIFT)                                                                                                          \
    X(LSHIFT)                                                                                                          \
    X(AND)                                                                                                             \
    X(OR)

typedef enum
{
#define X(x) OP_##x,
    OPS_
#undef X
} Op;

const char* opStr(Op op)
{
    switch (op)
    {
#define X(x)                                                                                                           \
    case OP_##x:                                                                                                       \
        return #x;
        OPS_
#undef X
            default : fprintf(stderr, "Corrupt op: %d\n", op);
        exit(EXIT_FAILURE);
    }
}

typedef struct
{
    char* name;
    Op op;
    char* inputA;
    char* inputB;
    char* description;
    bool value1Present;
    SignalStrength value1;
    SignalStrength bits;
} Gate;

void Gate_init(Gate* g, char* name, Op op, char* inputA, char* inputB)
{
    char* description;
    if (inputA == NULL)
    {
        description =
    }
}

int main(void)
{
    int errCode;
    size_t errOfs;
    pcre2_code* wirePattern = pcre2_compile((PCRE2_SPTR) "^[a-z]+$", PCRE2_ZERO_TERMINATED, 0, &errCode, &errOfs, NULL);
    if (errCode < 0)
    {
        char message[120];
        pcre2_get_error_message(errCode, (PCRE2_UCHAR*)message, 120);
        fprintf(stderr, "PCRE compile error: %s\n", message);
        return 1;
    }

    pcre2_code_free(wirePattern);
    return 0;
}
