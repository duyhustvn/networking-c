#ifndef THREAD_PROCESS_H_
#define THREAD_PROCESS_H_

#include <pthread.h>

#include "data_queue.h"

typedef struct threadData_ {
    int threadID;
    int countPackets;
    IPQueue* q;
} threadData;


void *process(void *threadArg);

#endif // THREAD_PROCESS_H_
