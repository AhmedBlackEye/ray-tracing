#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable/hit_record.h"
#include "hittable/hittable.h"
#include "material/material.h"
#include <stdbool.h>

#define INIT_CAP 10

// Bounding box structure (internal use)
typedef struct BoundingBox {
  Vec3 min;
  Vec3 max;
  bool valid;
} BoundingBox;

// Internal mesh structure (private - users don't see this)
typedef struct Mesh {
  DynArray *raw_triangles;
  BoundingBox bounds;
  bool bounds_dirty;
  Material *material;
} Mesh;

// === PUBLIC API === (Only these functions are exposed)

// Create a triangle mesh hittable
Hittable *mesh_create(Material *mat);

// Add triangle to mesh hittable
void mesh_add_triangle(Hittable *mesh_hittable, Vec3 v0, Vec3 v1, Vec3 v2);

// Print mesh hittable info
void mesh_print(const Hittable *hittable);

// === INTERSECTION FUNCTION === (for reuse)
// Ray-triangle intersection (reusable)
bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat);

#endif // TRIANGLE_MESH_H