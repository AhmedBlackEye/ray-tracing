#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"

typedef struct TriangleRaw {
  Vec3 v0, v1, v2;   // vertices
  Vec3 normal;       // pre-computed normal
  Vec3 edge1, edge2; // pre-computed edges
} TriangleRaw;

extern Hittable *triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2);
extern void triangle_raw_print(const TriangleRaw *tri);

// Used for triangle hittable as well.
extern bool triangle_hit(Hittable *self, Ray r, Interval t_bounds,
                         HitRecord *rec);
extern void triangle_destroy(void *self);

#endif // TRIANGLE_H