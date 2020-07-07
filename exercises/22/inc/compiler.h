#ifndef COMPILER_H
#define COMPILER_H

/*
    Functionlike macros and defines for easier work with GCC. (GNU99)

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <string.h>
#include <stdint.h>

/* With this macro, function will be always inlined */
#define ___inline___ inline __attribute__(( always_inline ))

/* use this instead of restrict */
#define ___restrict___ restrict

/* Use this macro if you want to write data of @size from @src to @dst */
#define WRITE_ONCE_SIZE(dst, src, size)                                     \
    do {                                                                    \
        _Pragma("GCC diagnostic push");                                     \
        _Pragma("GCC diagnostic ignored \"-Wstrict-aliasing\"");            \
        switch (size)                                                       \
        {                                                                   \
            case 1: *(uint8_t*)&dst = *(uint8_t*)&src; break;               \
            case 2: *(uint16_t*)&dst = *(uint16_t*)&src; break;             \
            case 4: *(uint32_t*)&dst = *(uint32_t*)&src; break;             \
            case 8: *(uint64_t*)&dst = *(uint64_t*)&src; break;             \
            default: (void)memcpy((void*)&dst, (void*)&src, (size_t)size);  \
        }                                                                   \
        _Pragma("GCC diagnostic pop");                                      \
    } while (0)                                                             \

#endif /* COMPILER_H */