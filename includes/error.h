#ifndef P1_PROJECT_ERROR_H
#define P1_PROJECT_ERROR_H

#include <stdlib.h>
#define NO_ERROR ((Error) {ErrNone, NULL})

typedef enum {
    ErrNone = 0,
    ErrAlloc,
    ErrUnexpectedEOF,
    ErrInvalidCriteria,
} ErrorKind;

typedef struct {
    ErrorKind kind;
    const char* msg;
} Error;

void printErr(Error);

// Creates an error without a msg.
static inline Error blankError(ErrorKind kind) {
    return (Error) {kind, NULL};
}

#endif
