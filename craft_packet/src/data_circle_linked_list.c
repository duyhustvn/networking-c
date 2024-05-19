#include "data_circle_linked_list.h"
#include <stdlib.h>


IPCircleLinkedList *IPCircleLinkedListAlloc() {
    IPCircleLinkedList *ll = (IPCircleLinkedList *) malloc(sizeof(IPCircleLinkedList));
    if (ll == NULL) {
        return NULL;
    }
    ll->last = NULL;
    ll->len = 0;
    return ll;
};

void IPCircleLinkedListFree(IPCircleLinkedList* ll) {
    free(ll);
};

void IPCicleLinkedListInsertAtTheEnd(IPCircleLinkedList *ll, Data *data) {
    if (ll->last == NULL) {
        ll->last = data;
        ll->last->next=data;
    }  else {
        data->next = ll->last->next;
        ll->last->next = data;
        ll->last = data;
    }
    ll->len++;
};

Data* IPCircleLinkedListNext(IPCircleLinkedList *ll) {
    Data* data = ll->last;
    ll->last = ll->last->next;
    return data;
};


