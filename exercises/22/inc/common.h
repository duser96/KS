#ifndef COMMON_H
#define COMMON_H

/*
    Set of common useful functionlike macros.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stdlib.h>
#include <generic.h>
#include <compiler.h>

/* get array size if declared on stack */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* after freeing memory, set pointer to NULL */
#define FREE(ptr_p)     \
    do {                \
        free(ptr_p);    \
        (ptr_p) = NULL; \
    } while (0)         \

/* swap A with B if both sizes are S */
#define SWAP(A, B, S)                       \
    do {                                    \
        BYTE buffer[S];                     \
        WRITE_ONCE_SIZE(buffer[0], A, S);   \
        WRITE_ONCE_SIZE(A, B, S);           \
        WRITE_ONCE_SIZE(B, buffer[0], S);   \
    } while (0)                             \

/* comparator */
typedef int (*compare_f)(const void* const first_p, const void* const second_p);

/* destructor */
typedef void (*destructor_f)(void* ptr_p);

#endif /* COMMON_H */