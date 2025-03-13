#include <stdio.h>
#include <assert.h>
#include <ekans-internals.h>
#include <ekans.h>
#include <environment.h>

void test_initialize_ekans() {
  initialize_ekans();
  {
    assert(head.next == &tail);
    assert(tail.prev == &head);
  }
  finalize_ekans();
  printf("[%s] passed\n", __FUNCTION__);
}

void test_create_number_value() {
  initialize_ekans();
  {
    ekans_value* const v = create_number_value(20250312);
    assert(v->type == number);
    assert(v->value.n == 20250312);
    assert(head.next == v);
    assert(v->prev == &head);
    assert(v->next == &tail);
  }
  finalize_ekans();
  printf("[%s] passed\n", __FUNCTION__);
}

void test_create_boolean_value() {
  initialize_ekans();
  {
    ekans_value* const v = create_boolean_value(true);
    assert(v->type == boolean);
    assert(v->value.b == true);
    assert(head.next == v);
    assert(v->prev == &head);
    assert(v->next == &tail);
  }
  finalize_ekans();
  printf("[%s] passed\n", __FUNCTION__);
}

void test_environment(void) {
  ekans_environment* const global_environment = create_environment(NULL, 1);
  free(global_environment->bindings);
  free(global_environment);
}

int main() {
  printf("=====================\n");
  test_initialize_ekans();
  test_create_number_value();
  test_create_boolean_value();
  test_environment();
  printf("=====================\n");
  printf("All tests passed!\n");
  return 0;
}
