// Copyright (c) 2025 Good Night, Good Morning and contributors (see Contributors.md)
// Licensed under the MIT License. See the LICENSE file in the project root for details.

#include <environment.h>

ekans_environment* create_environment(ekans_environment* const parent, const int size) {
  ekans_environment* const environment = (ekans_environment*)malloc(sizeof(ekans_environment));
  if (environment == NULL) {
    return NULL;
  }

  environment->bindings = (ekans_value**)calloc(size, sizeof(ekans_value*));
  if (environment->bindings == NULL) {
    free(environment);
    return NULL;
  }

  environment->binding_count = size;
  environment->parent        = parent;
  return environment;
}
