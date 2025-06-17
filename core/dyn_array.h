#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "generic_types.h"

typedef struct DynArray DynArray;

extern DynArray *dynarray_create(int initial_capacity, GPrintFn pelem,
                                 GDestroyFn delem);
extern void dynarray_destroy(DynArray *arr);
extern void dynarray_push(DynArray *arr, GVal elem);
extern GVal dynarray_pop(DynArray *arr);
GVal dynarray_pop_and_destroy(DynArray *arr);
extern GVal dynarray_get(const DynArray *arr, int index);
extern int dynarray_size(const DynArray *arr);
extern int dynarray_capacity(const DynArray *arr);
extern void dynarray_sort(DynArray *arr, GCmp compare);
void dynarray_partial_sort(DynArray *arr, int start, int end, GCmp compare);
extern void dynarray_print(const DynArray *arr);
