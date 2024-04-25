// The include section need to be in this order
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>
#include <stdlib.h>

#include "data_queue.h"

static void test_ip_enqueue(void **state) {

  struct IPQueue_ *q = (struct IPQueue_ *)malloc(sizeof(struct IPQueue_));

  struct Test {
    struct Data_ d;
  };

  struct Test tests[] = {
  {{"1", NULL, NULL}},
  {{"2", NULL, NULL}},
  {{"3", NULL, NULL}},
  {{"4", NULL, NULL}},
  };


  assert_null(q->top);
  assert_null(q->bot);

  IPEnqueue(q, &tests[0].d);
  assert_string_equal(q->top->ips, "1");
  assert_string_equal(q->bot->ips, "1");

  // bot -> top
  // 1 -> 2
  IPEnqueue(q, &tests[1].d);
  assert_string_equal(q->top->ips, "2");
  assert_string_equal(q->top->next->ips, "1");
  assert_null(q->top->next->next);

  // bot -> top
  // 1 -> 2 -> 3
  IPEnqueue(q, &tests[2].d);
  assert_string_equal(q->top->ips, "3");
  assert_string_equal(q->top->next->ips, "2");
  assert_string_equal(q->top->next->next->ips, "1");
  assert_null(q->top->next->next->next);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_ip_enqueue),
  };

  cmocka_set_message_output(CM_OUTPUT_SUBUNIT);

  return cmocka_run_group_tests(tests, NULL, NULL);
}
