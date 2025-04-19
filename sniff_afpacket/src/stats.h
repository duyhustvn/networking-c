#ifndef STATS_H_

#include "common.h"

typedef struct Stats_ {
    uint64_t packets_processed;
    uint64_t bytes_processed;
    
    struct timespec start_time;
    struct timespec end_time;
} Stats; 

void init_stats(Stats *stats);
void print_stats(Stats *stats);

#endif // STATS_H_
