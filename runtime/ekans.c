// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#include <assert.h>
#include <ekans-internals.h>
#include <ekans.h>
#include <stdio.h>
#include <stdlib.h>

#define EKANS_MARK_BITS 65536

// global variables

stack_slot* g_stack_slots = NULL;

ekans_value head;
ekans_value tail;

// runtime initialization/finalization

void initialize_ekans() {
  head.prev = NULL;
  head.next = &tail;
  tail.prev = &head;
  tail.next = NULL;
}

void finalize_ekans() {
  collect();
}

// value creation routines

ekans_value* create_number_value(int v) {
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  result->type    = number;
  result->value.n = v;
  append(result);
  return result;
}

ekans_value* create_boolean_value(bool v) {
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  result->type    = boolean;
  result->value.b = v;
  append(result);
  return result;
}

ekans_value* create_environment(ekans_value* parent, const int size) {
  assert(parent == NULL || is(parent, environment));
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  if (result == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for ekans_environment.\n");
    return NULL;
  }

  result->type             = environment;
  result->value.e.bindings = (ekans_value**)calloc(size, sizeof(ekans_value*));
  // printf("MAlloc %p\n", result->value.e.bindings);
  if (result->value.e.bindings == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for ekans environment bindings (size: %d).\n", size);
    free(result);
    return NULL;
  }

  result->value.e.binding_count = size;
  result->value.e.parent        = parent;

  append(result);
  return result;
}

ekans_value* create_closure(ekans_value* env, ekans_function function) {
  assert(is(env, environment));
  ekans_value* result = malloc(sizeof(ekans_value));
  // printf("Malloc %p\n", result);
  if (result == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for create_closure.\n");
    return NULL;
  }
  result->type             = closure;
  result->value.c.closure  = env;
  result->value.c.function = function;

  append(result);
  return result;
}

// Garbage collection routines

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
      if (is(cur, environment)) {
        // printf("FRee %p\n", cur->value.e.bindings);
        free(cur->value.e.bindings);
      }
      // freed += 1;
      cur->prev->next = cur->next;
      cur->next->prev = cur->prev;
      // printf("Free %p\n", cur);
      free(cur);
    }
    cur = next;
  }
  // no printf("[log] GC completed, freed = %d, reset = %d\n", freed, reset);
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
    if (is(obj, closure)) {
      mark_recursively(obj->value.c.closure);
    } else if (is(obj, environment)) {
      mark_recursively(obj->value.e.parent);
      for (int i = 0; i < obj->value.e.binding_count; i++) {
        mark_recursively(obj->value.e.bindings[i]);
      }
    }
  }
}

void mark_this(ekans_value* obj) {
  obj->type |= EKANS_MARK_BITS;
}

void reset_this(ekans_value* obj) {
  obj->type &= ~EKANS_MARK_BITS;
}

bool marked(ekans_value* obj) {
  return obj == NULL || (obj->type & EKANS_MARK_BITS) != 0;
}

// Accessors

void set_environment(ekans_value* env, int index, ekans_value* value) {
  assert(is(env, environment));
  assert(index < env->value.e.binding_count);
  env->value.e.bindings[index] = value;
}

ekans_value* get_environment(ekans_value* env, int levels_up, int index) {
  assert(is(env, environment));
  while (levels_up > 0 && env != NULL) {
    env = env->value.e.parent;
    assert(env != NULL);
    assert(is(env, environment));
    levels_up--;
  }

  assert(index < env->value.e.binding_count);
  return env->value.e.bindings[index];
}

ekans_value* closure_of(ekans_value* val) {
  assert(is(val, closure));
  return val->value.c.closure;
}

ekans_function function_of(ekans_value* val) {
  assert(is(val, closure));
  return val->value.c.function;
}

// Primitive runtime functions

bool is(ekans_value* obj, ekans_type type) {
  assert(obj);
  return ((obj->type | EKANS_MARK_BITS) == (type | EKANS_MARK_BITS));
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
      assert(!"print_ekans_value: unsupported");
    } break;
  }
}

int plus(ekans_value* environment, ekans_value** result) {
  int sum = 0;
  for (int i = 0; i < environment->value.e.binding_count; i++) {
    assert(environment->value.e.bindings[i] != NULL);
    if (environment->value.e.bindings[i]->type != number) {
      return 1;
    }
    sum += environment->value.e.bindings[i]->value.n;
  }
  *result = create_number_value(sum);
  return 0;
}