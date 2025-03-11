#pragma once

#include <ekans.h>

typedef enum { number, boolean } ekan_type;

typedef struct ekans_value {
  ekan_type type;
  union {
    int n;
    bool b;
  } value;
  struct ekans_value *prev;
  struct ekans_value *next;
} ekans_value;

void append(ekans_value *new_value);