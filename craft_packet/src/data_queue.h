#ifndef DATA_QUEUE_H_
#define DATA_QUEUE_H_

#include <stdio.h>
#include <stdint.h>

typedef struct Data_{
    char* ips;
    struct Data_ *next;
} Data;

// TODO: free queue after used
// add to rear
// remove from front
// next front -> rear
typedef struct IPQueue_{
    struct Data_ *front;
    struct Data_ *rear;

    uint32_t len;
} IPQueue;

void IPEnqueue(IPQueue*, struct Data_ *);
struct Data_ *IPDequeue(IPQueue*);
void IPQueueFree(IPQueue*);

#endif // DATA_QUEUE_H_
