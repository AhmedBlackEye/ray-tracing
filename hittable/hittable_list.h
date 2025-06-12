#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "core/dyn_array.h"
#include "core/ray.h"
#include "hittable.h"

bool hittables_hit(DynArray *hittables, Ray ray, double ray_tmin,
                   double ray_tmax, HitRecord *rec);

#endif