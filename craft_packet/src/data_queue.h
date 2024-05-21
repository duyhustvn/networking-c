#ifndef DATA_QUEUE_H_
#define DATA_QUEUE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "data.h"

// add to rear
// remove from front
// next front -> rear
typedef struct IPQueue_{
    struct Data_ *front;
    struct Data_ *rear;

    uint32_t len;

    pthread_mutex_t mutex;
} IPQueue;

void IPEnqueue(IPQueue*, struct Data_ *);
struct Data_ *IPDequeue(IPQueue*);
IPQueue *IPQueueAlloc(void);
void IPQueueFree(IPQueue*);
void IPQueueTraversal(IPQueue*);
bool IPQueueEmpty(IPQueue*);

#endif // DATA_QUEUE_H_
