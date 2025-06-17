#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/ray.h"
#include "hit_record.h"
#include "hittable.h"

extern Hittable *hittablelist_empty(void);
extern void hittablelist_add(Hittable *self, Hittable *new_hittable);
extern void hittablelist_print(const Hittable *hittable);

#endif
