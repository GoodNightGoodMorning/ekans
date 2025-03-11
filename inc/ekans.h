#pragma once

#include <stdio.h>

typedef enum { false, true } bool;

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

ekans_value *create_number_value(int v);

ekans_value *create_boolean_value(bool v);

void print_ekans_value(ekans_value *v);

void initialize_ekan();

void finalize_ekan();