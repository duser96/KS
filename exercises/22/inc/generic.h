#ifndef GENERIC_H
#define GENERIC_H

/*
    Primitive types.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stdint.h>

#define BYTE        uint8_t
#define HALF_WORD   uint16_t
#define WORD        uint32_t
#define DWORD       uint64_t
#define QWORD       __uint128_t

#define BYTE_SIZE       sizeof(BYTE)
#define HALF_WORD_SIZE  sizeof(HALF_WORD)
#define WORD_SIZE       sizeof(WORD)
#define DWORD_SIZE      sizeof(DWORD)
#define QWORD_SIZE      sizeof(QWORD)

#endif /* GENERIC_H */