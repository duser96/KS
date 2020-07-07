#include <memcpy.h>
#include <generic.h>
#include <compiler.h>
#include <immintrin.h>
#include <stdbool.h>

/* --------------------------------------- STATIC FUNCTION DECLARATION --------------------------------------------- */

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
static ___inline___ void* __memcpy_u8(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Check that addresses @dst_p and @src_p are aligned with sizeof(void*).

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.

    RETURN:
    true if addresses are aligned.
    false if addresses are not aligned.
*/
static ___inline___ bool __are_address_aligned(const void* const restrict dst_p, const void* const restrict src_p);

/*
    Memcpy per DWORD. If addresses @dst_p and @src_p are aligned with sizeof(void*) that we will be copying per 
    DWORD. If not, we will be copying per BYTE.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
static ___inline___ void* __memcpy_u64(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per DWORD with unroll loop with assignement. If addresses @dst_p and @src_p are aligned with sizeof(void*) 
    we will be copying per DWORD with unrool loop with assignement. If we don't fit with number of unroll DWORD's 
    chunks to copy, then __memcpy_u64 will be called. If addresses are not aligned, then __memcpy_u8 will be called.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
static ___inline___ void* __memcpy_u64_unroll(void* restrict dst_p, 
                                              const void* const restrict src_p, 
                                              const size_t size);

/*
    Memcpy per 32 BYTE's using Intel AVX/AVX2 intrinsics with not aligned addresses to 32 BYTE's. If we don't fit with
    number of chunks (32 BYTE's) then __memcpy_u64 will be called.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
static ___inline___ void* __memcpy_u256(void* restrict dst_p, const void* const restrict src_p, const size_t size);

/*
    Memcpy per 32 BYTE's with unroll loop with assignement using Intel AVX/AVX2 intrinsics with not aligned addresses
    to 32 BYTE's. If we don't fit with number of unroll chunks (8 * 32 BYTE's) then __memcpy_u256 will be called.

    PARAMS:
    @IN dst_p - pointer to the object to copy to.
    @IN src_p - pointer to the obecjt to copy from.
    @IN size - number of bytes to copy.

    RETURN:
    Copy of pointer to dst_p if success.
    NULL if failure.
*/
static ___inline___ void* __memcpy_u256_unroll(void* restrict dst_p, 
                                               const void* const restrict src_p, 
                                               const size_t size);

/* --------------------------------------- STATIC FUNCTION DEFINITION ---------------------------------------------- */

static ___inline___ void* __memcpy_u8(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    if (dst_p == NULL || src_p == NULL || size == 0)
    {
        return NULL;
    }

    BYTE* bdst_p = (BYTE*)dst_p;
    BYTE* bsrc_p = (BYTE*)src_p;

    for (size_t i = 0; i < size; ++i)
    {
        bdst_p[i] = bsrc_p[i];
    }

    return dst_p;
}

static ___inline___ bool __are_address_aligned(const void* const restrict dst_p, const void* const restrict src_p)
{
    const uintptr_t dst_addr = (const uintptr_t)dst_p;
    const uintptr_t src_addr = (const uintptr_t)src_p;

    if (dst_addr % sizeof(void*) == 0 && src_addr % sizeof(void*) == 0)
    {
        return true;
    }

    return false;
}

static ___inline___ void* __memcpy_u64(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    if (dst_p == NULL || src_p == NULL || size == 0)
    {
        return NULL;
    }

    if (__are_address_aligned(dst_p, src_p))
    {
        BYTE* bdst_p = (BYTE*)dst_p;
        const BYTE* const bsrc_p = (const BYTE* const)src_p;

        size_t idx = 0;
        const size_t nr_of_u64_chunks = size / DWORD_SIZE;
        const size_t nr_of_u8_chunks = size % DWORD_SIZE;

        for (size_t i = 0; i < nr_of_u64_chunks; ++i, idx += DWORD_SIZE)
        {
            *(DWORD*)&bdst_p[idx] = *(DWORD*)&bsrc_p[idx];
        }

        for (size_t i = 0; i < nr_of_u8_chunks; ++i, ++idx)
        {
            bdst_p[idx] = bsrc_p[idx];
        }

        return dst_p;
    }
    else
    {
        return __memcpy_u8(dst_p, src_p, size);
    }
}

static ___inline___ void* __memcpy_u64_unroll(void* restrict dst_p, 
                                              const void* const restrict src_p, 
                                              const size_t size)
{
    if (dst_p == NULL || src_p == NULL || size == 0)
    {
        return NULL;
    }

    if (__are_address_aligned(dst_p, src_p))
    {
        BYTE* bdst_p = (BYTE*)dst_p;
        const BYTE* const bsrc_p = (const BYTE* const)src_p;

        size_t _size = size;
        size_t idx = 0;

        const size_t nr_of_u64_unroll_chunks = ((_size) / DWORD_SIZE) / DWORD_SIZE;
        _size -= nr_of_u64_unroll_chunks * DWORD_SIZE * DWORD_SIZE;

        for (size_t j = 0; j < nr_of_u64_unroll_chunks; ++j, idx += DWORD_SIZE * DWORD_SIZE)
        {
            *(DWORD*)&bdst_p[idx + 0 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 0 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 1 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 1 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 2 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 2 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 3 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 3 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 4 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 4 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 5 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 5 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 6 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 6 * DWORD_SIZE];
            *(DWORD*)&bdst_p[idx + 7 * DWORD_SIZE] = *(DWORD*)&bsrc_p[idx + 7 * DWORD_SIZE];
        }

        if (_size > 0)
        {
            return __memcpy_u64((void*)&bdst_p[idx], (void*)&bsrc_p[idx], _size);
        }

        return dst_p;
    }
    else
    {
        return __memcpy_u8(dst_p, src_p, size);
    }
}

static ___inline___ void* __memcpy_u256(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    if (dst_p == NULL || src_p == NULL || size == 0)
    {
        return NULL;
    }

    BYTE* bdst_p = (BYTE*)dst_p;
    const BYTE* const bsrc_p = (const BYTE* const)src_p;

    size_t idx = 0;
    size_t _size = size;

    const size_t nr_of_u256_chunks = _size / sizeof(__m256i);
    _size -= nr_of_u256_chunks * sizeof(__m256i);

    for (size_t j = 0; j < nr_of_u256_chunks; ++j, idx += sizeof(__m256i))
    {
        __m256i v = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx], v);
    }

    if (_size > 0)
    {
        return __memcpy_u64((void*)&bdst_p[idx], (void*)&bsrc_p[idx], _size);
    }

    return dst_p;
}

static ___inline___ void* __memcpy_u256_unroll(void* restrict dst_p, 
                                               const void* const restrict src_p, 
                                               const size_t size)
{
    if (dst_p == NULL || src_p == NULL || size == 0)
    {
        return NULL;
    }

    BYTE* bdst_p = (BYTE*)dst_p;
    const BYTE* const bsrc_p = (const BYTE* const)src_p;

    size_t idx = 0;
    size_t _size = size;

    const size_t nr_of_u256_unroll_chunks = _size / sizeof(__m256i) / DWORD_SIZE;
    _size -= nr_of_u256_unroll_chunks * sizeof(__m256i) * DWORD_SIZE;

    __m256i ret;

    for (size_t j = 0; j < nr_of_u256_unroll_chunks; ++j, idx += sizeof(__m256i) * DWORD_SIZE)
    {
        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 0 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 0 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 1 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 1 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 2 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 2 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 3 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 3 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 4 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 4 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 5 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 5 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 6 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 6 * sizeof(__m256i)], ret);

        ret = _mm256_loadu_si256((const __m256i*)&bsrc_p[idx + 7 * sizeof(__m256i)]);
	    _mm256_storeu_si256((__m256i*)&bdst_p[idx + 7 * sizeof(__m256i)], ret);
    }

    if (_size > 0)
    {
        return __memcpy_u256((void*)&bdst_p[idx], (void*)&bsrc_p[idx], _size);
    }

    return dst_p;
}

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

void* memcpy_u8(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    return __memcpy_u8(dst_p, src_p, size);
}

void* memcpy_u64(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    return __memcpy_u64(dst_p, src_p, size);
}

void* memcpy_u64_unroll(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    return __memcpy_u64_unroll(dst_p, src_p, size);
}

void* memcpy_u256(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    return __memcpy_u256(dst_p, src_p, size);
}

void* memcpy_u256_unroll(void* restrict dst_p, const void* const restrict src_p, const size_t size)
{
    return __memcpy_u256_unroll(dst_p, src_p, size);
}