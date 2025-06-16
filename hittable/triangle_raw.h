#ifndef TRIANGLE_RAW_H
#define TRIANGLE_RAW_H

#include "core/vec3.h"
#include "material/material.h"

typedef struct TriangleRaw {
  Vec3 v0, v1, v2;   // Vertices
  Vec3 normal;       // Pre-computed normal
  Vec3 edge1, edge2; // Pre-computed edges (for when hit is needed)
} TriangleRaw;

// Pure data functions only
extern TriangleRaw *triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2);
extern void triangle_destroy(void *self);   // For dynarray
extern void triangle_raw_print(void *self); // For dynarray
extern bool triangle_raw_hit(const TriangleRaw *tri, Ray r, Interval t_bounds,
                             HitRecord *rec, Material *mat);

extern bool triangle_hit(Hittable *hittable, Ray r, Interval t_bounds,
                         HitRecord *rec);

// NO HIT FUNCTION - triangle_hittable handles ray intersection

#endif // TRIANGLE_RAW_H