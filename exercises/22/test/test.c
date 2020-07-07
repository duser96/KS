#include <memcpy.h>
#include <memset.h>
#include <benchmark.h>
#include <common.h>
#include <generic.h>
#include <stdlib.h>
#include <assert.h>

int main(void)
{
    const size_t sizes[] = {73, 1 << 20, 1 << 30};

    printf("TESTING MEMCPY\n");

    for (size_t i = 0; i < ARRAY_SIZE(sizes); ++i)
    {
        printf("bytes = %zu\n", sizes[i]);

        BYTE* dst_p = (BYTE*)calloc(sizes[i], sizeof(*dst_p));
        assert(dst_p != NULL);

        BYTE* src_p = (BYTE*)calloc(sizes[i], sizeof(*src_p));
        assert(src_p != NULL);

        for (size_t j = 0; j < sizes[i]; ++j)
        {
            src_p[j] = (BYTE)j % UINT8_MAX;
        }

        const size_t bytes = sizes[i] * sizeof(*src_p);

        MEASURE_FUNCTION(memcpy(dst_p, src_p, bytes), "memcpy");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        MEASURE_FUNCTION(memcpy_u8(dst_p, src_p, bytes), "memcpy_u8");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        MEASURE_FUNCTION(memcpy_u64(dst_p, src_p, bytes), "memcpy_u64");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        MEASURE_FUNCTION(memcpy_u64_unroll(dst_p, src_p, bytes), "memcpy_u64_unroll");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        MEASURE_FUNCTION(memcpy_u256(dst_p, src_p, bytes), "memcpy_u256");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        MEASURE_FUNCTION(memcpy_u256_unroll(dst_p, src_p, bytes), "memcpy_u256_unroll");
        assert(memcmp(dst_p, src_p, bytes) == 0);
        (void)memset(dst_p, 0, bytes);

        FREE(dst_p);
        FREE(src_p);

        printf("\n");
    }

    return 0;
}