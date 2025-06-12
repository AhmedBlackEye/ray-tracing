#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "generic_types.h"
#include "shared.h"

#define MIN_CAPACITY 8

struct DynArray {
  int size;
  int capacity;
  GPrintFn pelem;
  GDestroyFn delem;
  GVal *data;
};

DynArray *dynarray_create(int initial_capacity, GPrintFn pelem,
                          GDestroyFn delem) {
  assert(initial_capacity > 0);

  initial_capacity = MAX(initial_capacity, MIN_CAPACITY);

  DynArray *new = malloc(sizeof(struct DynArray));
  assert(new);
  new->data = malloc(sizeof(GVal) * initial_capacity);
  assert(new->data);
  new->size = 0;
  new->capacity = initial_capacity;
  new->pelem = pelem;
  new->delem = delem;
  return new;
}

void dynarray_destroy(DynArray *arr) {
  assert(arr != NULL);
  for (int i = 0; i < arr->size; i++) {
    arr->delem(arr->data[i]);
  }
  free(arr->data);
  free(arr);
}

int dynarray_size(const DynArray *arr) {
  assert(arr != NULL);
  return arr->size;
}

int dynarray_capacity(const DynArray *arr) {
  assert(arr != NULL);
  return arr->capacity;
}

void dynarray_push(DynArray *arr, GVal elem) {
  assert(arr != NULL);
  if (arr->size >= arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->data = realloc(arr->data, arr->capacity * sizeof(GVal));
    assert(arr->data);
  }
  arr->data[arr->size++] = elem;
}

GVal dynarray_pop(DynArray *arr) {
  assert(arr != NULL);
  assert(arr->size > 0);
  arr->size--;
  return arr->data[arr->size];
}

GVal dynarray_pop_and_free(DynArray *arr) {
  assert(arr != NULL);
  assert(arr->size > 0);
  arr->size--;
  GVal temp = arr->data[arr->size];
  arr->delem(arr->data[arr->size]);
  return temp;
}

void dynarray_clear(DynArray *arr) {
  assert(arr != NULL);
  for (int i = 0; i < arr->size; i++) {
    arr->delem(arr->data[i]);
  }
  arr->size = 0;
  arr->capacity = MIN_CAPACITY;
  arr->data = realloc(arr->data, arr->capacity * sizeof(GVal));
  assert(arr->data);
}

GVal dynarray_get(const DynArray *arr, int index) {
  assert(arr != NULL);
  assert(index >= 0 && index < arr->size);
  return arr->data[index];
}

void dynarray_print(const DynArray *arr) {
  assert(arr != NULL);

  if (arr->size == 0) {
    puts("[]");
    return;
  }

  putchar('[');
  for (int i = 0; i < arr->size; i++) {
    arr->pelem(arr->data[i]);
    if (i < arr->size - 1) {
      printf(", ");
    }
  }
  puts("]");
}