#ifndef THREAD_PROCESS_H_
#define THREAD_PROCESS_H_

#include <pthread.h>
#include <string.h>
#include <libnet.h>

#include "data_queue.h"
#include "craft_tcp.h"

typedef struct threadData_ {
    int threadID;
    int countPackets;
    IPQueue* q;

    uint32_t srcIp;
    uint8_t* srcMac;
    uint8_t* dstMac;

    libnet_t* l;
} threadData;


void *process(void *threadArg);

#endif // THREAD_PROCESS_H_
