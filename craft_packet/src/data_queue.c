#include "data_queue.h"


void IPEnqueue(IPQueue* q, struct Data_ *d) {
    if (d == NULL) {
        return;
    }

    // Queue is empty
    if (q->len == 0) {
        d->next = NULL;
        d->prev = NULL;
        q->top = q->bot = d;
        q->len = 1;
    } else {
        d->prev = NULL;
        d->next = q->top;
        q->top->prev = d;
        q->top = d;
        q->len++;
    }
};
