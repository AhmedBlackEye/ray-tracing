#ifndef TRIANGLE_HITTABLE_H
#define TRIANGLE_HITTABLE_H

#include "hittable.h"
#include "material/material.h"
#include "triangle_raw.h"

typedef struct TriangleHittable {
  TriangleRaw *triangle;
  Material material;

} TriangleHittable;

extern bool triangle_hit(const Hittable *hittable, Ray r, Interval t_bounds,
                         HitRecord *rec);
extern void triangle_hittable_print(const Hittable *hittable);
extern Hittable *triangle_hittable_create(Vec3 v0, Vec3 v1, Vec3 v2,
                                          Material mat);

#endif // TRIANGLE_HITTABLE_H