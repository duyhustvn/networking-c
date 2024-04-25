#ifndef DATA_QUEUE_H_
#define DATA_QUEUE_H_

#include <stdio.h>
#include <stdint.h>

typedef struct Data_{
    char* ips;
    struct Data_ *next;
    struct Data_ *prev;
} Data;

// TODO: free queue after used
// add to top
// remove from bottom
// next top->bottom
// prev bottom->top
typedef struct IPQueue_{
    struct Data_ *top;
    struct Data_ *bot;
    uint32_t len;
} IPQueue;

void IPEnqueue(IPQueue*, struct Data_ *);
struct IPQueue_ IPDequeue(IPQueue*);
void IPQueueFree(IPQueue*);

#endif // DATA_QUEUE_H_
