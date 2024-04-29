#include "data_queue.h"
#include <stdio.h>
#include <stdlib.h>


void IPEnqueue(IPQueue* q, struct Data_ *d) {
    if (d == NULL) {
        return;
    }

    d->next = NULL;
    // Queue is empty
    if (q->front == NULL) {
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


void IPQueueFree(IPQueue* q) {
    free(q);
};


void IPQueueTraversal(IPQueue* q) {
    printf("Number of elements in queue: %d\n", q->len);
    if (q->len > 0) {
        printf("front: %s\n", q->front->ips);
        printf("rear: %s\n", q->rear->ips);
    }

    struct Data_ *tmp = q->front;
    while (tmp != NULL) {
        printf("%s -> ", tmp->ips);
        tmp = tmp->next;
    }
    printf("NULL\n");
};


bool IPQueueEmpty(IPQueue* q) {
    return q->len == 0;
};
