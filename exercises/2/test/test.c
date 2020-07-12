#include <ldm_cm.h>
#include <common.h>
#include <assert.h>

/* --------------------------------------- STATIC FUNCTION DECLARATION --------------------------------------------- */

/*
    Unit test for functionlike macro writeToCm. Validated are cycles and correctness of memcpy.

    PARAMS:
    @IN void

    RETURN
    This is void function.
*/
static void test_write_to_cm(void);

/*
    Unit test for functionlike macro readFromCm. Validated are cycles and correctness of memcpy.

    PARAMS:
    @IN void

    RETURN
    This is void function.
*/
static void test_read_from_cm(void);

/*
    Unit test for functionlike macro ldmToCmCopy. Validated are cycles and correctness of memcpy.

    PARAMS:
    @IN void

    RETURN
    This is void function.
*/
static void test_ldm_to_cm_copy(void);

/*
    Unit test for functionlike macro cmToLdmCopy. Validated are cycles and correctness of memcpy.

    PARAMS:
    @IN void

    RETURN
    This is void function.
*/
static void test_cm_to_ldm_copy(void);

/* --------------------------------------- STATIC FUNCTION DEFINITION ---------------------------------------------- */

static void test_write_to_cm(void)
{
    cycles = 0;

    uint32_t a = 3;
    __cm uint32_t cm_b;

    writeToCm(cm_b, a);

    /* 
     * 0 cycles for ldm access
     * 4 cycles for ldm read
     * 30 cycles for cm access
     * 4 cycles for cm write
     */
    assert(cycles == 38UL);
    assert(ldmCmMemCmp(&a, &cm_b, sizeof(a)) == 0);
}

static void test_read_from_cm(void)
{
    cycles = 0;

    struct Pair
    {
        uint8_t key;
        uint8_t value;
    };

    __cm struct Pair cm_pair = { .key = 73, .value = 124 };
    struct Pair pair;

    readFromCm(pair, cm_pair);

    /*
     * 30 cycles for cm access
     * 0 cycles for ldm access
     * 2 cycles for cm read
     * 2 cycles for ldm write
     */
    assert(cycles == 34);
    assert(ldmCmMemCmp(&pair, &cm_pair, sizeof(pair)) == 0);
}

static void test_ldm_to_cm_copy(void)
{
    cycles = 0;

    uint64_t buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    __cm uint64_t cm_buffer[ARRAY_SIZE(buffer)];

    ldmToCmCopy(&cm_buffer[0], &buffer[0], sizeof(cm_buffer));

    /* 
     * 0 cycles for ldm access
     * 80 (10 * 8 bytes) cycles for ldm read
     * 30 cycles for cm access
     * 80 (10 * 8 bytes) cycles for cm write
     */
    assert(cycles == 190);
    assert(ldmCmMemCmp(&buffer[0], &cm_buffer[0], sizeof(buffer)) == 0);
}

static void test_cm_to_ldm_copy(void)
{
    cycles = 0;

    struct Record
    {
        uint16_t bitmap;
        uint64_t src;
        uint64_t dst;
    };

    __cm struct Record cm_record[] = 
    {
        { .bitmap = 0x0001, .src = 0, .dst = 10 },
        { .bitmap = 0x0002, .src = 1, .dst = 11 },
        { .bitmap = 0x0003, .src = 2, .dst = 12 },
    };
    struct Record record[ARRAY_SIZE(cm_record)];

    cmToLdmCopy(&record[0], &cm_record[0], sizeof(record));

    /*
     * 30 cycles for cm access
     * 3 * (8 + 8 + 8) cycles for cm read
     * 0 cycles for ldm access
     * 3 * (8 + 8 + 8) cycles for ldm write 
     */
    assert(cycles == 174);
    assert(ldmCmMemCmp(&record[0], &cm_record[0], sizeof(record)) == 0);
}

/* --------------------------------------------- MAIN FUNCTION ----------------------------------------------------- */

int main(void)
{
    test_write_to_cm();
    test_read_from_cm();
    test_ldm_to_cm_copy();
    test_cm_to_ldm_copy();

    return 0;
}
