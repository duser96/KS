#ifndef MEMCPY_H
#define MEMCPY_H

/*
    Implementation of many versions of memcpy.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h> /* size_t */

/*
    Memcpy per BYTE.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
void* memcpy_u8(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per DWORD.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
void* memcpy_u64(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per DWORD with unroll loop with assignement.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
void* memcpy_u64_unroll(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per 32 BYTE's using Intel AVX/AVX2 intrinsics.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
void* memcpy_u256(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per 32 BYTE's using Intel AVX/AVX2 intrinsics with unroll loop with assignement.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
void* memcpy_u256_unroll(void* restrict dst_p, const void* const restrict src_p, const size_t size);

#endif /* MEMCPY_H */