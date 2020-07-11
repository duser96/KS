#include <split_size_allocator.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

/* -------------------------------------------- FUNCTIONLIKE MACRO ------------------------------------------------- */

/* macro for calculating size of arrays allocated on stack */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/* ------------------------------------------------- STRUCTURES ---------------------------------------------------- */

struct Test_chunk_header
{
    uint32_t is_allocated : 1;
    uint32_t size_of : 31;
};

typedef struct Test_chunk_header Test_chunk_header;

/* ------------------------------------------- FUNCTION DECLARATION ------------------------------------------------ */

/*
    In this test case we want to allocate lot of memory chunks. After allocations we will go through available memory
    chunk to ensure they are properly allocated in memory.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_allocations(void);

/*
    In this test case we want to allocate lot of memory chunks and then deallocates all memory chunks. We expect that
    one memory chunk left as a not allocated, as after init memory.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_deallocations(void);

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

static void test_allocations(void)
{
    ssa_init();

    const size_t size_of_data[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    void* address[ARRAY_SIZE(size_of_data)] = {0};

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        address[i] = ssa_alloc(size_of_data[i]);
        assert(address[i] != NULL);
    }

    size_t offset = 0;
    Test_chunk_header* header_p = (Test_chunk_header*)ssa_get_address_from_memory(offset);

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        assert(header_p->is_allocated == true);
        assert(header_p->size_of == size_of_data[i] + sizeof(*header_p));

        offset += header_p->size_of;
        header_p = (Test_chunk_header*)ssa_get_address_from_memory(offset);
    }
}

static void test_deallocations(void)
{
    ssa_init();

    /* after lots of alloc -> dealloce we expect that we've got one chunk with size_of equals MEMORY_SIZE */
    const size_t offset = 0;
    const Test_chunk_header* const header_p = (Test_chunk_header*)ssa_get_address_from_memory(offset);

    /* allocate memory chunks in increasing order and then deallocate memory chunk in increasing order */
    const size_t size_of_data[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    void* address[ARRAY_SIZE(size_of_data)] = {0};

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        address[i] = ssa_alloc(size_of_data[i]);
        assert(address[i] != NULL);
    }

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        ssa_dealloc(address[i]);
    }

    assert(header_p->is_allocated == false);
    assert(header_p->size_of == MEMORY_SIZE);

    /* allocate memory chunks in increasing order and then deallocate memory chunk in decreasing order */
    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        address[i] = ssa_alloc(size_of_data[i]);
        assert(address[i] != NULL);
    }

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        ssa_dealloc(address[ARRAY_SIZE(size_of_data) - i - 1]);
    }

    assert(header_p->is_allocated == false);
    assert(header_p->size_of == MEMORY_SIZE);

    /* allocate memory chunks in increasing order and then deallocate memory chunk in given order */
    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        address[i] = ssa_alloc(size_of_data[i]);
        assert(address[i] != NULL);
    }

    const size_t index_to_dealloc[] = {0, 2, 4, 6, 8, 10, 9, 7, 5, 3, 1};
    assert(sizeof(index_to_dealloc) == sizeof(size_of_data));

    for (size_t i = 0; i < ARRAY_SIZE(size_of_data); ++i)
    {
        ssa_dealloc(address[index_to_dealloc[i]]);
    }

    assert(header_p->is_allocated == false);
    assert(header_p->size_of == MEMORY_SIZE);
}

/* ----------------------------------------------- MAIN FUNCTION --------------------------------------------------- */

int main(void)
{
    test_allocations();
    test_deallocations();

    return 0;
}

