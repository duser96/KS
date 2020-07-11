#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

/*
    Implementation of fixed-size allocator using static memory.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stdint.h>
#include <stddef.h>

/* default value, MEMORY_SIZE should be passed in compile time by -D option */
#ifndef MEMORY_SIZE
#define MEMORY_SIZE (1 << 20) /* 1 MB */
#endif

/* default page size in linux kernel is 4kB, could be passed in compile time by -D option */
#ifndef PAGE_SIZE
#define PAGE_SIZE (1 << 12)
#endif

#define SIZE_OF_CHUNK PAGE_SIZE /* 4kB */
#define BITS_IN_BYTE (1 << 3) /* 8 bits in byte */

/*
    This function memset memory and allocator metadata.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
void fsa_init(void);

/*
    This function implement simple allocator based on static memory. It is fixed-size allocator which means memory is 
    divided in fixed size chunks (RAM pages by default).

    PARAMS:
    @IN bytes - requested memory size in bytes.

    RETURN:
    @NULL if failure.
    @address if success.
*/
void* fsa_alloc(const size_t bytes);

/*
    This functon implement freeing memory. Function is responsible for get information about number of allocated chunks 
    under this address and set this value to zero. Then number of allocated bits will be set to zero started by given
    address.

    PARAMS:
    @IN addr_p - pointer to memory for deallocation.

    RETURN:
    This is void function.
*/
void fsa_dealloc(void* addr_p);

/*
    This function is responsible for print the following infromations to stdio:
    -number of allocated chunks
    -size of each allocated chunk
    -number of free chunks
    -size of each free chunk

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
void fsa_get_statistics(void);

/*
    Getter for size of memory array.

    PARAMS:
    @IN - void

    RETURN:
    Size of memory.
*/
size_t fsa_get_size_of_memory(void);

/*
    Getter for memory address. Returned pointer must be used as read only.

    PARAMS:
    @IN index - index in memory array.

    RETURN:
    Address from @(&memory[index]).
*/
void* fsa_get_address_from_memory(const size_t index);

/*
    Getter for size of available chunks array.

    PARAMS:
    @IN - void

    RETURN:
    Size of array.
*/
size_t fsa_get_size_of_available_chunks(void);

/*
    Getter for member of available chunks.

    PARAMS:
    @IN - index of array.

    RETURN:
    Bitmap with available chunks.
*/
uint8_t fsa_get_available_chunks(const size_t index);

/*
    Getter for size of number of chunks array.

    PARAMS:
    @IN - void

    RETURN:
    Size of array.
*/
size_t fsa_get_size_of_number_of_chunks(void);

/*
    Getter for member of number of chunks.

    PARAMS:
    @IN - index of array.

    RETURN:
    Number of chunks.
*/
uint8_t fsa_get_number_of_chunks(const size_t index);

#endif /* FIXED_SIZE_ALLOCATOR_H */