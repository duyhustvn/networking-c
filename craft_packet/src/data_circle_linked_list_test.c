// The include section need to be in this order
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>
#include <stdlib.h>

#include "data_circle_linked_list.h"


static void test_circle_linked_list_insert_at_the_end(void **state) {
    IPCircleLinkedList *ll = IPCircleLinkedListAlloc();

    struct Test {
        Data d;
    };

    struct Test tests[] = {
    {{"1", NULL}},
    {{"2", NULL}},
    {{"3", NULL}},
    {{"4", NULL}},
    };

    assert_null(ll->last);
    assert_int_equal(ll->len, 0);

    // 1  ->
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[0].d);
    assert_string_equal(ll->last->ip, "1");
    assert_string_equal(ll->last->next->ip, "1");
    assert_string_equal(ll->last->next->next->ip, "1");
    assert_string_equal(ll->last->next->next->next->ip, "1");


    // 1  -> 2 ->
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[1].d);
    assert_string_equal(ll->last->ip, "2");
    assert_string_equal(ll->last->next->ip, "1");
    assert_string_equal(ll->last->next->next->ip, "2");
    assert_string_equal(ll->last->next->next->next->ip, "1");

    // 1  -> 2 -> 3
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[2].d);
    assert_string_equal(ll->last->ip, "3");
    assert_string_equal(ll->last->next->ip, "1");
    assert_string_equal(ll->last->next->next->ip, "2");
    assert_string_equal(ll->last->next->next->next->ip, "3");
    assert_string_equal(ll->last->next->next->next->next->ip, "1");
    assert_string_equal(ll->last->next->next->next->next->next->ip, "2");

    // 1  -> 2 -> 3 -> 4
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[3].d);
    assert_string_equal(ll->last->ip, "4");
    assert_string_equal(ll->last->next->ip, "1");
    assert_string_equal(ll->last->next->next->ip, "2");
    assert_string_equal(ll->last->next->next->next->ip, "3");
    assert_string_equal(ll->last->next->next->next->next->ip, "4");
    assert_string_equal(ll->last->next->next->next->next->next->ip, "1");
    assert_string_equal(ll->last->next->next->next->next->next->next->ip, "2");
    assert_string_equal(ll->last->next->next->next->next->next->next->next->ip, "3");

    IPCircleLinkedListFree(ll);
}

static void test_circle_linked_list_next(void **state) {
    struct Test {
        Data d;
    };

    struct Test tests[] = {
    {{"1", NULL}},
    {{"2", NULL}},
    {{"3", NULL}},
    {{"4", NULL}},
    };

    IPCircleLinkedList *ll;
    ll = IPCircleLinkedListAlloc();
    assert_null(ll->last);
    assert_int_equal(ll->len, 0);
    // 1  ->
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[0].d);
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    IPCircleLinkedListFree(ll);

    // 1 -> 2
    ll = IPCircleLinkedListAlloc();
    assert_null(ll->last);
    assert_int_equal(ll->len, 0);
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[0].d);
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[1].d);
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "2");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "2");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    IPCircleLinkedListFree(ll);

    // 1 -> 2 -> 3
    ll = IPCircleLinkedListAlloc();
    assert_null(ll->last);
    assert_int_equal(ll->len, 0);
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[0].d);
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[1].d);
    IPCicleLinkedListInsertAtTheEnd(ll, &tests[2].d);
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "3");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "2");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "3");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "1");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "2");
    assert_string_equal(IPCircleLinkedListNext(ll)->ip, "3");
    IPCircleLinkedListFree(ll);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      // cmocka_unit_test(test_circle_linked_list_insert_at_the_end),
      cmocka_unit_test(test_circle_linked_list_next),
  };

  cmocka_set_message_output(CM_OUTPUT_SUBUNIT);

  return cmocka_run_group_tests(tests, NULL, NULL);
}
