#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct Triangle Triangle;

extern Hittable *triangle_create(Vec3 v0, Vec3 v1, Vec3 v2);
extern void triangle_print(const Hittable *hittable);
extern bool triangle_hit(Triangle *tri, Ray r, Interval t_bounds,
                         HitRecord *rec);

#endif // TRIANGLE_H