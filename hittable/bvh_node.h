#ifndef BVH_H
#define BVH_H

#include "hittable/hittable.h"

extern Hittable *bvhnode_create(Hittable *hittable_list);
extern void bvhnode_print(const Hittable *hittable);

#endif // BVH_H