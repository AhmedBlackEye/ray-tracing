#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "core/dyn_array.h"
#include "core/ray.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"



bool hittables_hit(DynArray *hittables, Ray ray, Interval t_bounds, HitRecord *rec);



#endif
