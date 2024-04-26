#include "data_queue.h"


void IPEnqueue(IPQueue* q, struct Data_ *d) {
    if (d == NULL) {
        return;
    }

    d->next = NULL;
    // Queue is empty
    if (q->len == 0) {
        q->rear = q->front = d;
        q->len = 1;
    } else {
        q->rear->next = d;
        q->rear = d;
        q->len++;
    }
};


struct Data_ *IPDequeue(IPQueue* q) {
    if (q->front == NULL) {
        return NULL;
    }

    struct Data_ *data = q->front;
    if (q->front->next == NULL) {
        // only have 1 element in queue
        q->front = NULL;
        q->rear = NULL;
    } else {
        // more than 1 elements
        q->front = q->front->next;
    }
    q->len--;

    data->next = NULL;
    return data;
};
