// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#pragma once

#include <ekans.h>

typedef enum {
  number,
  boolean
} ekans_type;

typedef struct ekans_environment {
  int                       binding_count;
  ekans_value**             bindings;
  struct ekans_environment* parent;
} ekans_environment;

typedef int (*ekans_function)(ekans_environment*, ekans_value**);

typedef struct ekans_closure {
  ekans_environment* closure;
  ekans_function     function;
} ekans_closure;

struct ekans_value {
  ekans_type type;
  union {
    int  n;
    bool b;
  } value;
  struct ekans_value* prev;
  struct ekans_value* next;
};

ekans_environment* create_environment(ekans_environment* parent, const int size);

int plus(ekans_environment* environment, ekans_value** result);

void set_environment(ekans_environment* env, int index, ekans_value* value);

ekans_value* get_environment(ekans_environment* env, int levels_up, int index);

ekans_environment* closure_of(ekans_value* val);

ekans_value* function_of(ekans_value* val);

typedef struct stack_slot {
  ekans_value**      slot;
  struct stack_slot* next;
} stack_slot;

extern stack_slot* g_stack_slots;

void append(ekans_value* new_value);

void mark();

bool marked(ekans_value* obj);

void mark_recursively(ekans_value* obj);

void mark_this(ekans_value* obj);

void sweep();

void reset_this(ekans_value* obj);
