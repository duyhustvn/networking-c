#include "stats.h"


void init_stats(Stats *stats) {
    stats->packets_processed = 0;
    stats->bytes_processed = 0;

    clock_gettime(CLOCK_MONOTONIC, &stats->start_time);
}

void print_stats(Stats *stats) {
    clock_gettime(CLOCK_MONOTONIC, &stats->end_time);

    double duration = (stats->end_time.tv_sec - stats->start_time.tv_sec) + 
        (stats->end_time.tv_nsec - stats->start_time.tv_nsec) / 1e9;

    double pps = (stats->packets_processed) / duration; // packet per second
    double mbps = (stats->bytes_processed * 8.0 / 1000000) / duration; // megabit per second
                                                                       
    printf("Duration: %.2f seconds\n", duration);
    printf("Packets processed: %.2lu\n", stats->packets_processed);
    printf("Throughput: %2f packets/second\n", pps);
    printf("Bandwidth: %2f Mpbs\n", mbps);
}
