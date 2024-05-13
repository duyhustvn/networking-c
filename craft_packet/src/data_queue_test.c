// The include section need to be in this order
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>
#include <stdlib.h>

#include "data_queue.h"

static void test_ip_enqueue_dequeue(void **state) {

  struct IPQueue_ *q = (struct IPQueue_ *)malloc(sizeof(struct IPQueue_));

  struct Test {
    struct Data_ d;
  };

  struct Test tests[] = {
  {{"1", NULL}},
  {{"2", NULL}},
  {{"3", NULL}},
  {{"4", NULL}},
  };


  assert_null(q->rear);
  assert_null(q->front);

  IPEnqueue(q, &tests[0].d);
  assert_string_equal(q->front->ip, "1");
  assert_string_equal(q->rear->ip, "1");

  // 1 -> 2
  IPEnqueue(q, &tests[1].d);
  assert_string_equal(q->front->ip, "1");
  assert_string_equal(q->front->next->ip, "2");
  assert_null(q->front->next->next);

  // 1 -> 2 -> 3
  IPEnqueue(q, &tests[2].d);
  assert_string_equal(q->front->ip, "1");
  assert_string_equal(q->front->next->ip, "2");
  assert_string_equal(q->front->next->next->ip, "3");
  assert_null(q->front->next->next->next);

  // // 1 -> 2 -> 3 -> 4
  IPEnqueue(q, &tests[3].d);
  assert_string_equal(q->front->ip, "1");
  assert_string_equal(q->front->next->ip, "2");
  assert_string_equal(q->front->next->next->ip, "3");
  assert_string_equal(q->front->next->next->next->ip, "4");
  assert_null(q->front->next->next->next->next);

  IPQueueTraversal(q);

  struct Data_ *data;
  data = IPDequeue(q);
  assert_string_equal(data->ip, "1");

  data = IPDequeue(q);
  assert_string_equal(data->ip, "2");

  data = IPDequeue(q);
  assert_string_equal(data->ip, "3");

  data = IPDequeue(q);
  assert_string_equal(data->ip, "4");

  data = IPDequeue(q);
  assert_null(data);

  IPQueueFree(q);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_ip_enqueue_dequeue),
  };

  cmocka_set_message_output(CM_OUTPUT_SUBUNIT);

  return cmocka_run_group_tests(tests, NULL, NULL);
}
