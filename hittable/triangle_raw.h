#ifndef TRIANGLE_RAW_H
#define TRIANGLE_RAW_H

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"

// Raw triangle struct (no pointers, pure data)
typedef struct TriangleRaw {
  Vec3 v0, v1, v2;   // Vertices
  Vec3 normal;       // Pre-computed normal
  Vec3 edge1, edge2; // Pre-computed edges
} TriangleRaw;

// Raw triangle functions (no Hittable dependencies!)
extern TriangleRaw triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2);
extern bool triangle_hit(const Hittable *tri, Ray r, Interval t_bounds,
                         HitRecord *rec);
extern void triangle_raw_print(const Triangle *tri);

#endif // TRIANGLE_RAW_H