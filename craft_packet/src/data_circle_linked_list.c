#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "data_circle_linked_list.h"
#include <err.h>



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
    if (ll->last) {
        Data *tmp = ll->last;
        Data *next;
        do {
            next = tmp->next;
            free(tmp);
            tmp = next;
        } while(tmp != ll->last);
    }
};

void IPCicleLinkedListInsertAtTheEnd(IPCircleLinkedList *ll, Data *data) {
    if (ll == NULL) {
        printf("FUCK %d", 1);
    } else {
        if (ll->last == NULL) {
            ll->last = data;
            ll->last->next=data;
        }  else {
            data->next = ll->last->next;
            ll->last->next = data;
            ll->last = data;
        }
        ll->len++;
        printf("len: %d\n\n", ll->len);
    }
};

Data* IPCircleLinkedListNext(IPCircleLinkedList *ll) {
    Data* data = ll->last;
    ll->last = ll->last->next;
    return data;
};


void IPCicleLinkedListTraversal(IPCircleLinkedList *ll) {
    if (ll->last != NULL) {
        Data *tmp = ll->last;
        do {
            printf("%s -> ", tmp->ip);
            tmp = tmp->next;
        }  while(tmp != ll->last);
    }
};
