#include <memset.h>
#include <generic.h>
#include <compiler.h>
#include <common.h>
#include <memcpy.h>
#include <immintrin.h>

#define L1_CACHE_SIZE (1 << 17) /* 128 KB */

/* --------------------------------------- STATIC FUNCTION DECLARATION --------------------------------------------- */

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
static ___inline___ void* __memset_u8(void* const dst_p, const int ch, const size_t size);

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
static ___inline___ void* __memset_u64(void* const dst_p, const int ch, const size_t size);

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
static ___inline___ void* __memset_u64_unroll(void* const dst_p, const int ch, const size_t size);

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
static ___inline___ void* __memset_l1(void* const dst_p, const int ch, const size_t size);

/* --------------------------------------- STATIC FUNCTION DEFINITION ---------------------------------------------- */

static ___inline___ void* __memset_u8(void* const dst_p, const int ch, const size_t size)
{
    if (dst_p == NULL || size == 0)
    {
        return NULL;
    }

    const BYTE fill_byte = (const BYTE)ch;
    BYTE* const bdst_p = (BYTE* const)dst_p;

    for (size_t byte_idx = 0; byte_idx < size; ++byte_idx)
    {
        bdst_p[byte_idx] = fill_byte;
    }

    return dst_p;
}

static ___inline___ void* __memset_u64(void* const dst_p, const int ch, const size_t size)
{
    if (dst_p == NULL || size == 0)
    {
        return NULL;
    }

    DWORD fill_dword;

    if (ch == 0)
    {
        fill_dword = 0;
    }
    else
    {
        BYTE* tmp_p = (BYTE*)&fill_dword;

        for (size_t byte_idx = 0; byte_idx < DWORD_SIZE; ++byte_idx)
        {
            tmp_p[byte_idx] = (BYTE)ch;
        }
    }

    BYTE* const bdst_p = (BYTE* const)dst_p;
    const size_t nr_of_u64_chunks = size / DWORD_SIZE;
    const size_t nr_of_u8_chunks = size % DWORD_SIZE;

    size_t idx = 0;

    for (size_t i = 0; i < nr_of_u64_chunks; ++i, idx += DWORD_SIZE)
    {
        *(DWORD*)&bdst_p[idx] = fill_dword;
    }

    for (size_t i = 0; i < nr_of_u8_chunks; ++i, ++idx)
    {
        bdst_p[idx] = (BYTE)ch;
    }

    return dst_p;
}

static ___inline___ void* __memset_u64_unroll(void* const dst_p, const int ch, const size_t size)
{
    if (dst_p == NULL || size == 0)
    {
        return NULL;
    }

    DWORD fill_dword;

    if (ch == 0)
    {
        fill_dword = 0;
    }
    else
    {
        BYTE* tmp_p = (BYTE*)&fill_dword;

        for (size_t byte_idx = 0; byte_idx < DWORD_SIZE; ++byte_idx)
        {
            tmp_p[byte_idx] = (BYTE)ch;
        }
    }

    size_t _size = size;
    BYTE* const bdst_p = (BYTE* const)dst_p;

    const size_t nr_of_u64_unroll_chunks = (_size / DWORD_SIZE) / DWORD_SIZE;
    _size -= nr_of_u64_unroll_chunks * DWORD_SIZE * DWORD_SIZE;

    size_t idx = 0;

    for (size_t i = 0; i < nr_of_u64_unroll_chunks; ++i, idx += DWORD_SIZE * DWORD_SIZE)
    {
        *(DWORD*)&bdst_p[idx + 0 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 1 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 2 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 3 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 4 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 5 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 6 * DWORD_SIZE] = fill_dword;
        *(DWORD*)&bdst_p[idx + 7 * DWORD_SIZE] = fill_dword;
    }

    if (_size > 0)
    {
        return __memset_u64((void*)&bdst_p[idx], ch, _size);
    }

    return dst_p;
}

static ___inline___ void* __memset_l1(void* const dst_p, const int ch, const size_t size)
{
    if (dst_p == NULL || size == 0)
    {
        return NULL;
    }

    if (size > L1_CACHE_SIZE)
    {
        BYTE buffer[L1_CACHE_SIZE] = {0};

        if (ch != 0)
        {
            (void)memset_u64_unroll((void*)&buffer[0], ch, ARRAY_SIZE(buffer));
        }

        BYTE* bdst_p = (BYTE*)dst_p;
        size_t idx = 0;
        size_t _size = size;

        const size_t nr_of_l1_chunks = _size / L1_CACHE_SIZE;
        _size -= nr_of_l1_chunks * L1_CACHE_SIZE;

        for (size_t i = 0; i < nr_of_l1_chunks; ++i, idx += L1_CACHE_SIZE)
        {
            (void)memcpy_u256_unroll((void*)&bdst_p[idx], (void*)&buffer[0], ARRAY_SIZE(buffer));
        }

        return dst_p;
    }

    return memset_u64_unroll(dst_p, ch, size);
}

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

void* memset_u8(void* const dst_p, const int ch, const size_t size)
{
    return __memset_u8(dst_p, ch, size);
}

void* memset_u64(void* const dst_p, const int ch, const size_t size)
{
    return __memset_u64(dst_p, ch, size);
}

void* memset_u64_unroll(void* const dst_p, const int ch, const size_t size)
{
    return __memset_u64_unroll(dst_p, ch, size);
}

void* memset_l1(void* const dst_p, const int ch, const size_t size)
{
    return __memset_l1(dst_p, ch, size);
}