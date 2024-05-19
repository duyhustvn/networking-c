#ifndef DATA_CIRCLE_LINKED_LIST_H_
#define DATA_CIRCLE_LINKED_LIST_H_


#include <stdint.h>

#include "data.h"

typedef struct IPCircleLinkedList_ {
    struct Data_ *last;

    uint32_t len;
} IPCircleLinkedList;

IPCircleLinkedList *IPCircleLinkedListAlloc();
void IPCircleLinkedListFree(IPCircleLinkedList*);
void IPCicleLinkedListInsertAtTheEnd(IPCircleLinkedList*, Data*);
Data* Next(IPCircleLinkedList*);

#endif // DATA_CIRCLE_LINKED_LIST_H_
