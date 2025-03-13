// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#include <assert.h>
#include <ekans-internals.h>
#include <ekans.h>
#include <stdio.h>
#include <stdlib.h>

#define EKANS_MARK_BITS 65536

stack_slot* g_stack_slots = NULL;

ekans_value head;
ekans_value tail;

void initialize_ekans() {
  head.prev = NULL;
  head.next = &tail;
  tail.prev = &head;
  tail.next = NULL;
}

ekans_value* create_number_value(int v) {
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  append(result);
  result->type    = number;
  result->value.n = v;
  return result;
}

ekans_value* create_boolean_value(bool v) {
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  append(result);
  result->type    = boolean;
  result->value.b = v;
  return result;
}

void print_ekans_value(ekans_value* v) {
  switch (v->type) {
    case number: {
      printf("%d\n", v->value.n);
    } break;
    case boolean: {
      switch (v->value.b) {
        case true: {
          printf("#t\n");
        } break;
        case false: {
          printf("#f\n");
        } break;
        default: {
          assert(!"print_ekans_value: unknown boolean value");
        } break;
      }
    } break;
    default: {
      assert(!"print_ekans_value: unknown type");
    } break;
  }
}

void push_stack_slot(ekans_value** slot) {
  stack_slot* top = malloc(sizeof(stack_slot));
  // printf("malloc %p\n", top);
  top->slot     = slot;
  top->next     = g_stack_slots;
  g_stack_slots = top;
}

void pop_stack_slot(int count) {
  for (int i = 0; i < count; i++) {
    stack_slot* top = g_stack_slots;
    g_stack_slots   = top->next;
    // printf("free %p\n", top);
    free(top);
  }
}

void collect() {
  mark();
  sweep();
}

void mark() {
  stack_slot* cur = g_stack_slots;
  while (cur != NULL) {
    ekans_value* obj = *(cur->slot);
    if (obj) {
      mark_recursively(obj);
    }
    cur = cur->next;
  }
}

void sweep() {
  ekans_value* cur = head.next;
  // int freed = 0;
  // int reset = 0;
  while (cur != &tail) {
    ekans_value* next = cur->next;
    if (marked(cur)) {
      // reset += 1;
      reset_this(cur);
    } else {
      // freed += 1;
      cur->prev->next = cur->next;
      cur->next->prev = cur->prev;
      // printf("Free %p\n", cur);
      free(cur);
    }
    cur = next;
  }
  // printf("[log] Garbage Collection completed, freed = %d, reset = %d\n",
  // freed, reset);
}

void finalize_ekans() {
  collect();
}

void append(ekans_value* new_value) {
  new_value->prev       = tail.prev;
  new_value->next       = &tail;
  new_value->prev->next = new_value;
  new_value->next->prev = new_value;
}

void mark_recursively(ekans_value* obj) {
  if (!marked(obj)) {
    mark_this(obj);
    // TODO, recursively mark referenced values when we start producing
    // complex values
  }
}

void mark_this(ekans_value* obj) {
  obj->type |= EKANS_MARK_BITS;
}

void reset_this(ekans_value* obj) {
  obj->type &= ~EKANS_MARK_BITS;
}

bool marked(ekans_value* obj) {
  return (obj->type & EKANS_MARK_BITS) != 0;
}

ekans_environment* create_environment(ekans_environment* parent, const int size) {
  ekans_environment* const environment = (ekans_environment*)malloc(sizeof(ekans_environment));
  if (environment == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for ekans_environment.\n");
    return NULL;
  }

  environment->bindings = (ekans_value**)calloc(size, sizeof(ekans_value*));
  if (environment->bindings == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for ekans environment bindings (size: %d).\n", size);
    free(environment);
    return NULL;
  }

  environment->binding_count = size;
  environment->parent        = parent;
  return environment;
}

int plus(ekans_environment* environment, ekans_value** result) {
  int sum = 0;
  for (int i = 0; i < environment->binding_count; i++) {
    if (environment->bindings[i] == NULL || environment->bindings[i]->type != number) {
      return 1;
    }
    sum += environment->bindings[i]->value.n;
  }
  *result = create_number_value(sum);
  return 0;
}

void set_environment(ekans_environment* env, int index, ekans_value* value) {
  if (index >= 0 && index < env->binding_count) {
    env->bindings[index] = value;
  }
}

ekans_value* get_environment(ekans_environment* env, int levels_up, int index) {
  while (levels_up > 0 && env != NULL) {
    env = env->parent;
    levels_up--;
  }

  if (env != NULL && index >= 0 && index < env->binding_count) {
    return env->bindings[index];
  }
  return NULL;
}

ekans_environment* closure_of(ekans_value* val) {
  return ((ekans_closure*)val)->closure;
}

ekans_value* function_of(ekans_value* val) {
  return val;
}
