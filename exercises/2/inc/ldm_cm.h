#ifndef LDM_CM_H
#define LDM_CM_H

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

/* -------------------------------------------- GLOBAL VARIABLES --------------------------------------------------- */

static size_t cycles;

/* ------------------------------------------------- DEFINES ------------------------------------------------------- */

#define LDM_LATENCY_CC 0
#define LDM_THP_PER_BYTE_CC 1

#define CM_LATENCY_CC 30
#define CM_THP_PER_BYTE_CC 1

/* to check address space run: sparse -Waddress-space -Wcast-to-as *.c */

/* for sparse only */
#ifdef __CHECKER__
#define __cm  __attribute__ (( address_space (1)))
#define __force_cast  __attribute__ ((force))
#else /* in any compiler mode do nothing */
#define __cm
#define __force_cast
#endif

/* this macro should be used only to trick compiler in special cases like below */
#define __force_cast_to_ldm __force_cast
#define __force_cast_to_cm __force_cast __cm

/* ------------------------------------------- FUNCTIONLIKE MACRO -------------------------------------------------- */

#define cmMalloc(size) (__force_cast_to_cm void*)malloc(size)
#define cmFree(ptr) free((__force_cast_to_ldm void*)ptr)
#define ldmCmMemCmp(ldm_ptr, cm_ptr, size) memcmp(ldm_ptr, (const __force_cast_to_ldm void* const)cm_ptr, size)

#define writeToCm(cm_dst, ldm_src) \
    do { \
        const size_t size = sizeof(cm_dst); \
        cycles += LDM_LATENCY_CC + (size * LDM_THP_PER_BYTE_CC) + CM_LATENCY_CC + (size * CM_THP_PER_BYTE_CC); \
        (void)memcpy((__force_cast_to_ldm void* restrict)&cm_dst, &ldm_src, size); \
    } while (0)

#define readFromCm(ldm_dst, cm_src) \
    do { \
        const size_t size = sizeof(ldm_dst); \
        cycles += CM_LATENCY_CC + (size * CM_THP_PER_BYTE_CC) + LDM_LATENCY_CC + (size * LDM_THP_PER_BYTE_CC); \
        (void)memcpy(&ldm_dst, (__force_cast_to_ldm void* restrict)&cm_src, size); \
    } while (0)

#define ldmToCmCopy(cm_dst, ldm_src, size) \
    do { \
        cycles += LDM_LATENCY_CC + ((size) * LDM_THP_PER_BYTE_CC) + CM_LATENCY_CC + ((size) * CM_THP_PER_BYTE_CC); \
        (void)memcpy((__force_cast_to_ldm void* restrict)cm_dst, ldm_src, size); \
    } while (0)

#define cmToLdmCopy(ldm_dst, cm_src, size) \
    do { \
        cycles += CM_LATENCY_CC + ((size) * CM_THP_PER_BYTE_CC) + LDM_LATENCY_CC + ((size) * LDM_THP_PER_BYTE_CC); \
        (void)memcpy(ldm_dst, (__force_cast_to_ldm void* restrict)cm_src, size); \
    } while (0)
    
#endif /* LDM_CM_H */
