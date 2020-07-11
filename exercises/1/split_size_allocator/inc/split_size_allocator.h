#ifndef SPLIT_SIZE_ALLOCATOR_H
#define SPLIT_SIZE_ALLOCATOR_H

/*
    Implementation of split-size allocator using static memory.

    author: Kamil Kielbasa
    email: dusergithub@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>

/* default value, MEMORY_SIZE should be passed in compile time by -D option */
#ifndef MEMORY_SIZE
#define MEMORY_SIZE (1 << 20) /* 1 MB */
#endif

typedef struct Chunk_header Chunk_header;

/*
    This function is responsible for set proper values for first available memory chunk.

    PARAMS:
    @IN - void

    RETURN:
    This is void function.
*/
void ssa_init(void);

/*
    This function implement simple allocator based on static memory. It is split-size allocator which means memory is 
    devided by requested size of bytes plus sizeof(header). 

    PARAMS:
    @bytes - requested memory size in bytes.

    RETURN:
    @NULL if failure.
    @address if success.
*/
void* ssa_alloc(const size_t bytes);

/*
    This functon implement freeing memory. Function is responsible for set bit in freeing chunk as not allocated and go 
    through all available chunks and merge two chunks abreast if they are not allocated.

    PARAMS:
    @addr_p - pointer to memory for freeing.

    RETURN:
    This is void function.
*/
void ssa_dealloc(void* addr_p);

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
void ssa_get_statistics(void);

/*
    Getter for size of memory array.

    PARAMS:
    @IN - void

    RETURN:
    Size of memory.
*/
size_t ssa_get_size_of_memory(void);

/*
    Getter for memory address. Returned pointer must be used as read only.

    PARAMS:
    @IN index - index in memory array.

    RETURN:
    Address from @(&memory[index]).
*/
void* ssa_get_address_from_memory(const size_t index);

#endif /* SPLIT_SIZE_ALLOCATOR_H */