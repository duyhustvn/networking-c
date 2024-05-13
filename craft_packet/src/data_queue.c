#include "data_queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


IPQueue *IPQueueAlloc(void) {
    IPQueue *q = (IPQueue *)malloc(sizeof(IPQueue));
    if (q == NULL) {
        return NULL;
    }
    pthread_mutex_init(&q->mutex, NULL);
    return q;
};

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
    pthread_mutex_lock(&q->mutex);
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
    pthread_mutex_unlock(&q->mutex);
    return data;
};


void IPQueueFree(IPQueue* q) {
    pthread_mutex_destroy(&q->mutex);
    free(q);
};


void IPQueueTraversal(IPQueue* q) {
    printf("Number of elements in queue: %d\n", q->len);
    if (q->front != NULL) {
        printf("front: %s\n", q->front->ip);
        printf("rear: %s\n", q->rear->ip);
    }

    struct Data_ *tmp = q->front;
    while (tmp != NULL) {
        printf("%s -> ", tmp->ip);
        tmp = tmp->next;
    }
    printf("NULL\n");
};


bool IPQueueEmpty(IPQueue* q) {
    if (q->len <= 0 || q->front == NULL || q->rear == NULL) {
        printf("QUEUE IS EMPTY\n");
        printf("q->len: %d\n", q->len);
        if (q->front == NULL) {
            printf("front is NULL\n");
        }
        if (q->rear == NULL) {
            printf("rear is NULL\n");
        }
        return true;
    };
    return false;
};
