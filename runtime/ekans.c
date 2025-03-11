#include <assert.h>
#include <ekans-internals.h>
#include <ekans.h>
#include <stdio.h>
#include <stdlib.h>

ekan_value head;
ekan_value tail;

void initialize_ekan() {
  head.prev = NULL;
  head.next = &tail;
  tail.prev = &head;
  tail.next = NULL;
}

ekan_value *create_number_value(int v) {
  ekan_value *result = malloc(sizeof(ekan_value));
  append(result);
  result->type = number;
  result->value.n = v;
  return result;
}

ekan_value *create_boolean_value(bool v) {
  ekan_value *result = malloc(sizeof(ekan_value));
  append(result);
  result->type = boolean;
  result->value.b = v;
  return result;
}

void print_ekans_value(ekan_value *v) {
  switch (v->type) {
  case number:
    printf("%d\n", v->value.n);
    break;
  case boolean:
    switch (v->value.b) {
    case true:
      printf("#t\n");
      break;
    case false:
      printf("#f\n");
      break;
    default:
      assert(!"print_ekans_value: unknown boolean value");
    }
    break;
  default:
    assert(!"print_ekans_value: unknown type");
  }
}

void finalize_ekan() {
  ekan_value *cur = head.next;
  while (cur != &tail) {
    cur = cur->next;
    free(cur->prev);
  }
}

void append(ekan_value *new_value) {
  new_value->prev = tail.prev;
  new_value->next = &tail;
  new_value->prev->next = new_value;
  new_value->next->prev = new_value;
}