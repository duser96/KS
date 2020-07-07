#ifndef BENCHMARK_H
#define BENCHMARK_H

// #define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define CLOCK_TYPE CLOCK_MONOTONIC
#define MEASURE_FUNCTION(func, label)                                                   \
    do {                                                                                \
        struct timespec start;                                                          \
        struct timespec end;                                                            \
        double timeTaken;                                                               \
                                                                                        \
        clock_gettime(CLOCK_TYPE, &start);                                              \
        (void)func;                                                                     \
        clock_gettime(CLOCK_TYPE, &end);                                                \
                                                                                        \
        timeTaken = (double)(end.tv_sec - start.tv_sec) * 1e9;                          \
        timeTaken = (double)(timeTaken + (double)(end.tv_nsec - start.tv_nsec)) * 1e-9; \
                                                                                        \
        printf(label " time = %lf[s]\n", timeTaken);                                    \
    } while (0)

#endif /* BENCHMARK_H */