#include <fixed_size_allocator.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------- FUNCTIONLIKE MACRO ------------------------------------------------- */

/* macro for calculating size of arrays allocated on stack */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

/* --------------------------------------------- STATIC VARIABLES -------------------------------------------------- */

/* memory for allocations */
static uint8_t memory[MEMORY_SIZE];

/* 
    This array keep informations about free or allocated chunks of memory. 
    1* - 0 = free chunk of memory; 
    2* - 1 = allocated chunk of memory
*/
static uint8_t available_chunks[(MEMORY_SIZE / SIZE_OF_CHUNK) / BITS_IN_BYTE];

/* This array keey information about number of allocated chunks */
static uint8_t number_of_chunks[(MEMORY_SIZE / SIZE_OF_CHUNK)];

/* ------------------------------------------------ STRUCTURES ----------------------------------------------------- */

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
	@IN void

	RETURN:
	@Pointer to Memory_statistic if success
*/
static Memory_statistic* __memory_get_statistic(void);

/* --------------------------------------- STATIC FUNCTION DEFINITION ---------------------------------------------- */

static Memory_statistic* __memory_get_statistic(void)
{
	Memory_statistic* ms_p = calloc(1, sizeof(*ms_p));
	assert(ms_p != NULL);

	size_t sizes[(MEMORY_SIZE / SIZE_OF_CHUNK)] = {0};

	for (size_t i = 0; i < ARRAY_SIZE(number_of_chunks); ++i)
	{
		if (number_of_chunks[i] != 0)
		{
			sizes[ms_p->nr_of_allocated_chunks] = number_of_chunks[i];
			++ms_p->nr_of_allocated_chunks;
		}
	}

	if (ms_p->nr_of_allocated_chunks > 0)
	{
		ms_p->size_of_allocated_chunks_p = (size_t*)calloc(ms_p->nr_of_allocated_chunks,
													  	   sizeof(*ms_p->size_of_allocated_chunks_p));
		assert(ms_p->size_of_allocated_chunks_p != NULL);

		(void)memcpy(ms_p->size_of_allocated_chunks_p, 
					 &sizes[0], 
					 ms_p->nr_of_allocated_chunks * sizeof(*ms_p->size_of_allocated_chunks_p));
	}

	if ((MEMORY_SIZE / SIZE_OF_CHUNK) - ms_p->nr_of_allocated_chunks > 0)
	{
		(void)memset(&sizes[0], 0, sizeof(sizes));
		size_t accumulator = 0;

		for (size_t i = 0; i < ARRAY_SIZE(number_of_chunks); ++i)
		{
			if (number_of_chunks[i] != 0)
			{
				if (accumulator > 0)
				{
					sizes[ms_p->nr_of_free_chunks] = accumulator;
					++ms_p->nr_of_free_chunks;
					accumulator = 0;
				}

				i += (size_t)number_of_chunks[i] - 1;
			}
			else
			{
				++accumulator;
			}
		}

		if (accumulator != 0)
		{
			sizes[ms_p->nr_of_free_chunks] = accumulator;
			++ms_p->nr_of_free_chunks;
		}

		ms_p->size_of_free_chunks_p = (size_t*)calloc(ms_p->nr_of_free_chunks,
													  sizeof(*ms_p->size_of_free_chunks_p));
		assert(ms_p->size_of_free_chunks_p != NULL);

		(void)memcpy(ms_p->size_of_free_chunks_p, 
					 &sizes[0], 
					 ms_p->nr_of_free_chunks * sizeof(*ms_p->size_of_free_chunks_p));
	}

	return ms_p;
}

/* -------------------------------------------- FUNCTION DEFINITION ------------------------------------------------ */

void fsa_init(void)
{
	(void)memset(&memory[0], 0, sizeof(memory));
	(void)memset(&available_chunks[0], 0, sizeof(available_chunks));
	(void)memset(&number_of_chunks[0], 0, sizeof(number_of_chunks));
}

void* fsa_alloc(const size_t bytes)
{
	/* check if requested bytes if bigger than zero */
	if (bytes == 0)
	{
		return NULL;
	}

	/* calculate requested chunks */
	const size_t req_chunks = (bytes / SIZE_OF_CHUNK) + 1;

	/* check if requested number of chunks is smaller than 8 chunks */
	if (req_chunks > BITS_IN_BYTE)
	{
		return NULL;
	}

	uint8_t mask = 0;

	/* create proper mask for allocation */
	for (size_t i = 0; i < req_chunks; ++i)
	{
		mask <<= 1;
		mask |= 1;
	}

	uint8_t copy_mask8 = 0;
	uint16_t copy_mask16 = 0;

	/* looking for available memory */
	for (size_t i = 0; i < ARRAY_SIZE(available_chunks); ++i)
	{
		copy_mask8 = mask;

		/* take a look in inside byte */
		for (size_t j = 0; j < BITS_IN_BYTE - req_chunks + 1; ++j)
		{
			/* check for empty chunks */
			if ((available_chunks[i] & copy_mask8) == 0)
			{
				/* mark these chunks already allocated */
				available_chunks[i] |= copy_mask8;

				/* calculated allocated bit in metadata array */
				const size_t index = (i * BITS_IN_BYTE) + j; 

				/* save number of allocated chunks */
				number_of_chunks[index] = (uint8_t)req_chunks;

				const size_t offset = index * SIZE_OF_CHUNK;
				return (void*)&memory[offset];
			}

			copy_mask8 <<= 1;
		}

		/* take a look between two metadata array index */
		if (i + 1 < ARRAY_SIZE(available_chunks))
		{
			copy_mask16 = (uint16_t)mask;

			/* shift bits to the end of uint8_t */
			for (size_t j = 0; j < BITS_IN_BYTE - req_chunks; ++j)
			{
				copy_mask16 <<= 1;
			}

			uint16_t* memory_p = (uint16_t*)&available_chunks[i];

			for (size_t j = 0; j < req_chunks; ++j)
			{
				if ((*memory_p & copy_mask16) == 0)
				{
					/* mark these chunks already allocated */
					*memory_p |= copy_mask16;

					/* calculated allocated bit in metadata array */
					const size_t index = (i * BITS_IN_BYTE) + (BITS_IN_BYTE - req_chunks) + j;

					/* save number of allocated chunks */
					number_of_chunks[index] = (uint8_t)req_chunks;

					const size_t offset = index * SIZE_OF_CHUNK;
					return (void*)&memory[offset];
				}

				/* shift bits for check new possible free bits */
				copy_mask16 <<= 1;
			}
		}
	}

	return NULL;
}

void fsa_dealloc(void* addr_p)
{
	if (addr_p == NULL)
	{
		return;
	}

	if ((uint8_t*)addr_p < &memory[0] || (uint8_t*)addr_p > &memory[MEMORY_SIZE - 1])
	{
		return;
	}

	const ptrdiff_t diff = (uint8_t*)addr_p - &memory[0];
	const size_t index = diff / SIZE_OF_CHUNK;
	const size_t allocated_chunks = number_of_chunks[index];

	if (allocated_chunks == 0 || allocated_chunks > BITS_IN_BYTE)
	{
		return;
	}

	uint16_t mask = 0;

	for (size_t i = 0; i < allocated_chunks; ++i)
	{
		mask <<= 1;
		mask |= 1;
	}

	/* calculate first allocated chunk in metadata bitmap */
	uint16_t* metadata_p = (uint16_t*)&available_chunks[(index / BITS_IN_BYTE)];

	for (size_t i = 0; i < (index % BITS_IN_BYTE); ++i)
	{
		mask <<= 1;
	}

	*metadata_p ^= mask;
	number_of_chunks[index] = 0;
}

void fsa_get_statistics(void)
{
    const Memory_statistic* const ms_p = __memory_get_statistic();

    printf("number of allocated chunks = %zu\n", ms_p->nr_of_allocated_chunks);

	if (ms_p->nr_of_allocated_chunks != 0)
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

    printf("number of free chunks = %zu\n", ms_p->nr_of_free_chunks);

	if (ms_p->nr_of_free_chunks != 0)
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

	if (ms_p->nr_of_allocated_chunks != 0)
	{
		free((void*)ms_p->size_of_allocated_chunks_p);
	}

	if (ms_p->nr_of_free_chunks != 0)
	{
		free((void*)ms_p->size_of_free_chunks_p);
	}

    free((void*)ms_p);
}

size_t fsa_get_size_of_memory(void)
{
	return ARRAY_SIZE(memory);
}

void* fsa_get_address_from_memory(const size_t index)
{
	return (void*)&memory[index];
}

size_t fsa_get_size_of_available_chunks(void)
{
	return ARRAY_SIZE(available_chunks);
}

uint8_t fsa_get_available_chunks(const size_t index)
{
	return available_chunks[index];
}

size_t fsa_get_size_of_number_of_chunks(void)
{
	return ARRAY_SIZE(number_of_chunks);
}

uint8_t fsa_get_number_of_chunks(const size_t index)
{
	return number_of_chunks[index];
}