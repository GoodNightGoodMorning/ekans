// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#pragma once

#include <ekans-internals.h>
#include <stdlib.h>

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

ekans_environment* create_environment(ekans_environment* const parent, const int size);

int plus(ekans_environment* const environment, ekans_value** const result);
