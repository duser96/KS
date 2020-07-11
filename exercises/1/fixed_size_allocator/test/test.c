#include <fixed_size_allocator.h>
#include <assert.h>
#include <string.h>

/* -------------------------------------------- FUNCTIONLIKE MACRO ------------------------------------------------- */

/* macro for calculating size of arrays allocated on stack */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/* ------------------------------------------- FUNCTION DECLARATION ------------------------------------------------ */

/*
    In this test case we want to allocate pages inside metadata array index many times:

    - 1 chunk * 8.
    - 2 chunk * 4.
    - 4 chunk * 2.
    - 8 chunk * 1.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_allocations(void);

/*
    In this test case we want to allocate pages between metadata array index.

    array index:      n        n+1       n+2
    array  bits: |1111 1110|0011 1110|0111 1111|...

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_allocations_between_index(void);

/*
    In this test case we want to 'free' memory which means set proper bits responsible for pages to zero and set value
    to zero in array which contain number of allocated pages. This test case frees metadata inside metadata array 
    index and between these indexes.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_frees(void);

/*
    In this test case we want to make sure that after free between allocated chunks, we will be able to allocate once 
    again the same freed chunks.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
static void test_allocations_find_empty_bits(void);

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

static void test_allocations(void)
{
    fsa_init();

    const size_t nr_of_allocations = 15;

    const size_t expt_available_chunks_index[] = 
    {
        0,
        1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3,
    };
    const uint8_t expt_available_chunks[] = 
    {  
        0xff, 
        0x0f, 0xff, 
        0x03, 0x0f, 0x3f, 0xff, 
        0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
    };

    const size_t expt_number_of_chunks_index[] = 
    {
        0,
        8, 12,
        16, 18, 20, 22,
        24, 25, 26, 27, 28, 29, 30, 31,
    };

    const uint8_t expt_number_of_chunks[] = 
    {
        8, 
        4, 4, 
        2, 2, 2, 2, 
        1, 1, 1, 1, 1, 1, 1, 1,
    };

    const size_t bytes_to_allocate[] =
    {
        (7 * PAGE_SIZE) + 1,
        (3 * PAGE_SIZE) + 1, (3 * PAGE_SIZE) + 1,
        (1 * PAGE_SIZE) + 1, (1 * PAGE_SIZE) + 1, (1 * PAGE_SIZE) + 1, (1 * PAGE_SIZE) + 1,
        (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1,
        (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1, (0 * PAGE_SIZE) + 1,
    };

    for (size_t i = 0; i < nr_of_allocations; ++i)
    {
        void* ptr_p = fsa_alloc(bytes_to_allocate[i]);

        assert(ptr_p != NULL);
        assert(ptr_p == fsa_get_address_from_memory(0 + expt_number_of_chunks_index[i] * PAGE_SIZE));
        assert(fsa_get_number_of_chunks(expt_number_of_chunks_index[i]) == expt_number_of_chunks[i]);
        assert(fsa_get_available_chunks(expt_available_chunks_index[i]) == expt_available_chunks[i]);

        ptr_p = NULL;
    }
}

static void test_allocations_between_index(void)
{
    fsa_init();

    void* ptr_p = NULL;

    /*
        array index:      0         1         2
        array  bits: |0000 0000|0000 0000|0000 0000|...
    */

    ptr_p = fsa_alloc((6 * PAGE_SIZE) + 1);
    assert(ptr_p != NULL);
    assert(ptr_p == fsa_get_address_from_memory(0 + 0 * PAGE_SIZE));
    assert(fsa_get_available_chunks(0) == 0x7f);

    ptr_p = NULL;

    /*
        array index:      0         1         2
        array  bits: |1111 1110|0000 0000|0000 0000|...
    */

    ptr_p = fsa_alloc((1 * PAGE_SIZE) + 1);
    assert(ptr_p != NULL);
    assert(ptr_p == fsa_get_address_from_memory(0 + 7 * PAGE_SIZE));
    assert(fsa_get_available_chunks(0) == 0xff);
    assert(fsa_get_available_chunks(1) == 0x01);
    ptr_p = NULL;

    /*
        array index:      0         1         2
        array  bits: |1111 1111|1000 0000|0000 0000|...
    */

    ptr_p = fsa_alloc((7 * PAGE_SIZE) + 1);
    assert(ptr_p != NULL);
    assert(ptr_p == fsa_get_address_from_memory(0 + 9 * PAGE_SIZE));
    assert(fsa_get_available_chunks(0) == 0xff);
    assert(fsa_get_available_chunks(1) == 0xff);
    assert(fsa_get_available_chunks(2) == 0x01);
    ptr_p = NULL;

    /*
        array index:      0         1         2
        array  bits: |1111 1111|1111 1111|1000 0000|...
    */
}

static void test_frees(void)
{
    fsa_init();

    const size_t bytes_to_allocate[] = 
    {
        (2 * PAGE_SIZE) + 1, 
        (2 * PAGE_SIZE) + 1, 
        (2 * PAGE_SIZE) + 1,
    };

    const size_t expt_available_chunks_index[] = 
    {
        0, 0, 0,
    };
    const uint8_t expt_available_chunks[] = 
    {  
        0x07, 0x3f, 0xff, 
    };

    const size_t expt_number_of_chunks_index[] = 
    {
        0, 3, 6,
    };

    const uint8_t expt_number_of_chunks[] = 
    {
        3, 3, 3,
    };

    void* address[ARRAY_SIZE(bytes_to_allocate)] = {0};

    for (size_t i = 0; i < ARRAY_SIZE(bytes_to_allocate); ++i)
    {
        address[i] = fsa_alloc(bytes_to_allocate[i]);

        assert(address[i] != NULL);
        assert(address[i] == fsa_get_address_from_memory(0 + expt_number_of_chunks_index[i] * PAGE_SIZE));
        assert(fsa_get_number_of_chunks(expt_number_of_chunks_index[i]) == expt_number_of_chunks[i]);
        assert(fsa_get_available_chunks(expt_available_chunks_index[i]) == expt_available_chunks[i]);

        if (i == ARRAY_SIZE(bytes_to_allocate) - 1)
        {
            assert(fsa_get_available_chunks(1) == 0x01);
        }
    }

    /*
        Right now we've got the following allocated bits:

         array  bits:   |1111 1111|1000 0000|...
                         |||| |||| |
        address[0] ----->|||| |||| |   
                            | |||| |
        address[1] -------->| |||| |
                                || |
        address[2] ------------>|| |

        1* - free address[1], expected number_of_chunks[3] = 0, |1110 0011|1000 0000|...
        2* - free address[0], expected number_of_chunks[0] = 0, |0000 0011|1000 0000|...
        3* - free address[2], expected number_of_chunks[6] = 0, |0000 0000|0000 0000|...
    */

    fsa_dealloc(address[1]);
    assert(fsa_get_number_of_chunks(3) == 0);
    assert(fsa_get_available_chunks(0) == 0xc7);
    assert(fsa_get_available_chunks(1) == 0x01);

    fsa_dealloc(address[0]);
    assert(fsa_get_number_of_chunks(0) == 0);
    assert(fsa_get_available_chunks(0) == 0xc0);
    assert(fsa_get_available_chunks(1) == 0x01);

    fsa_dealloc(address[2]);
    assert(fsa_get_number_of_chunks(6) == 0);
    assert(fsa_get_available_chunks(0) == 0x00);
    assert(fsa_get_available_chunks(1) == 0x00);
}

static void test_allocations_find_empty_bits(void)
{
    fsa_init();

    void* address[3] = {0};

    for (size_t i = 0; i < ARRAY_SIZE(address); ++i)
    {
        address[i] = fsa_alloc((1 * PAGE_SIZE) + 1);
        assert(address[i] != NULL);
    }

    /*
        array index:      0        
        array  bits: |1111 1100|...
    */

    fsa_dealloc(address[1]);
    assert(fsa_get_number_of_chunks(2) == 0);
    assert(fsa_get_available_chunks(0) == 0x33);

    /*
        array index:      0        
        array  bits: |1100 1100|...
    */

    address[1] = fsa_alloc((1 * PAGE_SIZE) + 1);
    assert(address[1] != NULL);
    assert(address[1] == fsa_get_address_from_memory(0 + 2 * PAGE_SIZE));
    assert(fsa_get_number_of_chunks(2) == 2);
    assert(fsa_get_available_chunks(0) == 0x3f);

    /*
        array index:      0        
        array  bits: |1111 1100|...
    */

    fsa_init();
    (void)memset(&address[0], 0, sizeof(address));

    for (size_t i = 0; i < ARRAY_SIZE(address); ++i)
    {
        address[i] = fsa_alloc((5 * PAGE_SIZE) + 1);
        assert(address[i] != NULL);
    }

    /*
        array index:      0         1
        array  bits: |1111 1111|1111 1111|...
    */

    fsa_dealloc(address[1]);
    assert(fsa_get_number_of_chunks(6) == 0);
    assert(fsa_get_available_chunks(0) == 0x3f);
    assert(fsa_get_available_chunks(1) == 0xf0);

    /*
        array index:      0         1
        array  bits: |1111 1100|0000 1111|...
    */

    address[1] = fsa_alloc((5 * PAGE_SIZE) + 1);
    assert(address[1] != NULL);
    assert(address[1] == fsa_get_address_from_memory(0 + 6 * PAGE_SIZE));
    assert(fsa_get_number_of_chunks(6) == 6);
    assert(fsa_get_available_chunks(0) == 0xff);
    assert(fsa_get_available_chunks(1) == 0xff);

    /*
        array index:      0         1
        array  bits: |1111 1111|1111 1111|...
    */
}

/* ----------------------------------------------- MAIN FUNCTION --------------------------------------------------- */

int main(void)
{
    test_allocations();
    test_allocations_between_index();
    test_frees();
    test_allocations_find_empty_bits();

    return 0;
}

