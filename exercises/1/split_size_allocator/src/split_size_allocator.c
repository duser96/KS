#include <split_size_allocator.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------- FUNCTIONLIKE MACRO ------------------------------------------------- */

/* macro for calculating size of arrays allocated on stack */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/* --------------------------------------------- STATIC VARIABLES -------------------------------------------------- */

/* memory for allocations */
static uint8_t memory[MEMORY_SIZE];

/* ------------------------------------------------ STRUCTURES ----------------------------------------------------- */

struct Chunk_header
{
    uint32_t is_allocated : 1;
    uint32_t size_of : 31;
};

struct Memory_statistic
{
    size_t nr_of_allocated_chunks;
    size_t* size_of_allocated_chunks_p;

    size_t nr_of_free_chunks;
    size_t* size_of_free_chunks_p;
};

typedef struct Memory_statistic Memory_statistic;

/* --------------------------------------- STATIC FUNCTION DECLARATION --------------------------------------------- */

/*
	This function is responsible for collect informations about allocated/free memory chunks.

	PARAMS:
	@IN - void

	RETURN:
	@Pointer to Memory_statistic
*/
static Memory_statistic* __memory_get_statistic(void);

/* --------------------------------------- STATIC FUNCTION DEFINITION ---------------------------------------------- */

static Memory_statistic* __memory_get_statistic(void)
{
    Memory_statistic* ms_p = (Memory_statistic*)calloc(1, sizeof(*ms_p));
    assert(ms_p != NULL);

    Chunk_header* header_p;

    for (size_t offset = 0; offset < MEMORY_SIZE; offset += header_p->size_of)
    {
        header_p = (Chunk_header*)&memory[offset];

        if (header_p->is_allocated == true)
        {
            ++ms_p->nr_of_allocated_chunks;
        }
        else
        {
            ++ms_p->nr_of_free_chunks;
        }
    }

    if (ms_p->nr_of_allocated_chunks > 0)
    {
        ms_p->size_of_allocated_chunks_p = (size_t*)calloc(ms_p->nr_of_allocated_chunks, 
                                                           sizeof(*ms_p->size_of_allocated_chunks_p));
        assert(ms_p->size_of_allocated_chunks_p != NULL);
    }

    if (ms_p->nr_of_free_chunks > 0)
    {
        ms_p->size_of_free_chunks_p = (size_t*)calloc(ms_p->nr_of_free_chunks, 
                                                      sizeof(*ms_p->size_of_free_chunks_p));
        assert(ms_p->size_of_free_chunks_p != NULL);
    }

    size_t nr_of_allocated_chunks = 0;
    size_t nr_of_free_chunks = 0;

    for (size_t offset = 0; offset < MEMORY_SIZE; offset += header_p->size_of)
    {
        header_p = (Chunk_header*)&memory[offset];

        if (header_p->is_allocated == true)
        {
            ms_p->size_of_allocated_chunks_p[nr_of_allocated_chunks] = (size_t)header_p->size_of;
            ++nr_of_allocated_chunks;
        }
        else
        {
            ms_p->size_of_free_chunks_p[nr_of_free_chunks] = (size_t)header_p->size_of;
            ++nr_of_free_chunks;
        }
    }

    return ms_p;
}

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

void ssa_init(void)
{
    (void)memset(&memory[0], 0, sizeof(memory));

    Chunk_header* const header_p = (Chunk_header*)&memory[0];

    header_p->is_allocated = false;
    header_p->size_of = sizeof(memory);
}

void* ssa_alloc(const size_t bytes)
{
    if (bytes == 0 || bytes > (MEMORY_SIZE - sizeof(Chunk_header)))
    {
        return NULL;
    }

    Chunk_header* header_p = NULL;
    const size_t req_memory = bytes + sizeof(*header_p);
    
    /* first fit allocation */
    for (size_t offset = 0; offset < MEMORY_SIZE; offset += header_p->size_of)
    {
        header_p = (Chunk_header*)&memory[offset];

        if (header_p->is_allocated == false && header_p->size_of > req_memory)
        {
            const size_t old_size_of = header_p->size_of;

            header_p->is_allocated = true;
            header_p->size_of = req_memory;

            header_p = (Chunk_header*)&memory[offset + req_memory];

            header_p->is_allocated = false;
            header_p->size_of = old_size_of - req_memory;

            return (void*)&memory[offset + sizeof(*header_p)];
        }
    }

    return NULL;
}

void ssa_dealloc(void* addr_p)
{
    if (addr_p == NULL)
    {
        return;
    }

    /* mark header under @addr_p as free */
    ((Chunk_header*)((uint8_t*)addr_p - sizeof(Chunk_header)))->is_allocated = false;

    size_t number_of_chunks = 0;

    Chunk_header* curr_header_p;
    Chunk_header* next_header_p;

    /* go through memory chunks and merge them if possible */
    for (size_t offset = 0; offset < MEMORY_SIZE; offset += curr_header_p->size_of, ++number_of_chunks)
    {
        curr_header_p = (Chunk_header*)&memory[offset];

        if (offset + (size_t)curr_header_p->size_of > MEMORY_SIZE)
        {
            break;
        }
        else
        {
            next_header_p = (Chunk_header*)&memory[offset + curr_header_p->size_of];

            if (curr_header_p->is_allocated == false && next_header_p->is_allocated == false)
            {
                curr_header_p->size_of += next_header_p->size_of;
            }
        }
    }

    /* This logic in necessary for merge two last free blocks into one */
    if (number_of_chunks == 2)
    {
        curr_header_p = (Chunk_header*)&memory[0];
        next_header_p = (Chunk_header*)&memory[0 + curr_header_p->size_of];

        if (curr_header_p->is_allocated == false && next_header_p->is_allocated == false)
        {
            curr_header_p->size_of += next_header_p->size_of; 
        }
    }
}

void ssa_get_statistics(void)
{
    const Memory_statistic* const ms_p = __memory_get_statistic();

    printf("number of allocated chunks = %zu\n", ms_p->nr_of_allocated_chunks);

    if (ms_p->nr_of_allocated_chunks > 0)
    {
        double size_of_med = 0;

        printf("sizes: ");

        for (size_t i = 0; i < ms_p->nr_of_allocated_chunks; ++i)
        {
            size_of_med += (double)ms_p->size_of_allocated_chunks_p[i];

            printf("%zu ", ms_p->size_of_allocated_chunks_p[i]);
        }

        size_of_med /= (double)ms_p->nr_of_allocated_chunks;

        printf("\nsize_of medium = %lf\n", size_of_med);
    }

    printf("number of frees chunks = %zu\n", ms_p->nr_of_free_chunks);

    if (ms_p->nr_of_free_chunks > 0)
    {
        double size_of_med = 0;

        printf("sizes: ");

        for (size_t i = 0; i < ms_p->nr_of_free_chunks; ++i)
        {
            size_of_med += (double)ms_p->size_of_free_chunks_p[i];

            printf("%zu ", ms_p->size_of_free_chunks_p[i]);
        }

        size_of_med /= (double)ms_p->nr_of_free_chunks;
        
        printf("\nsize_of medium = %lf\n", size_of_med);
    }

    if (ms_p->nr_of_allocated_chunks > 0)
    {
        free(ms_p->size_of_allocated_chunks_p);
    }

    if (ms_p->nr_of_free_chunks > 0)
    {
        free(ms_p->size_of_free_chunks_p);
    }

    free((void*)ms_p);
}

size_t ssa_get_size_of_memory(void)
{
    return ARRAY_SIZE(memory);
}

void* ssa_get_address_from_memory(const size_t index)
{
    return (void*)&memory[index];
}
