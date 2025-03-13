// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#include <environment.h>

ekans_environment* create_environment(ekans_environment* const parent, const int size) {
  ekans_environment* const environment = (ekans_environment*)malloc(sizeof(ekans_environment));
  environment->binding_count           = size;
  environment->bindings                = (ekans_value**)calloc(size, sizeof(ekans_value*));
  environment->parent                  = parent;
  return environment;
}
