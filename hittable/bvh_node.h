#ifndef BVH_H
#define BVH_H

#include "hittable/hittable.h"

extern Hittable *bvhnode_create(Hittable *hittable_list);
extern bool bvhnode_hit(Hittable *self, Ray ray, Interval t_bounds,
                        HitRecord *rec);
extern void bvhnode_print(const Hittable *hittable);

#endif // BVH_H