#ifndef MEMSET_H
#define MEMSET_H

/*
    Implementation of many versions of memset.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h> /* size_t */

/*
    Memset per BYTE.

    PARAMS:
    @IN dst_p - pointer to the object to fill.
    @IN ch - fill byte.
    @IN size - number of bytes to fill.

    RETURN:
    A copy of dst_p if success.
    NULL if failure.
*/
void* memset_u8(void* const dst_p, const int ch, const size_t size);

/*
    Memset per DWORD.

    PARAMS:
    @IN dst_p - pointer to the object to fill.
    @IN ch - fill byte.
    @IN size - number of bytes to fill.

    RETURN:
    A copy of dst_p if success.
    NULL if failure.
*/
void* memset_u64(void* const dst_p, const int ch, const size_t size);

/*
    Memset per DWORD with unroll loop with assignement.

    PARAMS:
    @IN dst_p - pointer to the object to fill.
    @IN ch - fill byte.
    @IN size - number of bytes to fill.

    RETURN:
    A copy of dst_p if success.
    NULL if failure.
*/
void* memset_u64_unroll(void* const dst_p, const int ch, const size_t size);

/*
    Memset per L1 cache line size.

    PARAMS:
    @IN dst_p - pointer to the object to fill.
    @IN ch - fill byte.
    @IN size - number of bytes to fill.

    RETURN:
    A copy of dst_p if success.
    NULL if failure.
*/
void* memset_l1(void* const dst_p, const int ch, const size_t size);

#endif /* MEMSET_H */