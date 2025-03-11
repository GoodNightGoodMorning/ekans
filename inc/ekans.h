#pragma once

#include <stdio.h>

typedef enum { false, true } bool;

typedef enum { number, boolean } ekan_type;

typedef struct ekan_value {
  ekan_type type;
  union {
    int n;
    bool b;
  } value;
  struct ekan_value *prev;
  struct ekan_value *next;
} ekan_value;

ekan_value *create_number_value(int v);

ekan_value *create_boolean_value(bool v);

void print_ekans_value(ekan_value *v);

void initialize_ekan();

void finalize_ekan();