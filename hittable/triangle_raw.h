#ifndef TRIANGLE_RAW_H
#define TRIANGLE_RAW_H

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "material/material.h"

// Raw triangle struct (no pointers, pure data)
typedef struct TriangleRaw {
  Vec3 v0, v1, v2;   // Vertices
  Vec3 normal;       // Pre-computed normal
  Vec3 edge1, edge2; // Pre-computed edges
  Material material;
} TriangleRaw;

// Raw triangle functions (no Hittable dependencies!)
extern TriangleRaw *triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2,
                                        Material mat);
extern void triangle_destroy(void *self);
extern void triangle_raw_print(const TriangleRaw *tri);

#endif // TRIANGLE_RAW_H