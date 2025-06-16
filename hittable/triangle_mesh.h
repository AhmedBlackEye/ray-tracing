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
#define TRIANGLE_MESH_EPSILON 1e-13

// Bounding box structure
typedef struct BoundingBox {
  Vec3 min;
  Vec3 max;
  bool valid;
} BoundingBox;

// Triangle mesh structure
typedef struct Mesh {
  DynArray *raw_triangles;
  BoundingBox bounds;
  bool bounds_dirty;
  Material *material;
} Mesh;

// === MESH DATA FUNCTIONS ===
// Create mesh data structure
Mesh *mesh_create_data(Material *mat);

// Add triangle to mesh
void mesh_add_triangle(Mesh *mesh, Vec3 v0, Vec3 v1, Vec3 v2);

// Destroy mesh data
void mesh_destroy_data(Mesh *mesh);

// Print mesh info
void mesh_print_data(const Mesh *mesh);

// === HITTABLE WRAPPER FUNCTIONS ===
// Create hittable wrapper for mesh
Hittable *mesh_create_hittable(Mesh *mesh, Material *mat);

// Print hittable mesh
void mesh_print(const Hittable *hittable);

// === BOUNDING BOX FUNCTIONS ===
// Create empty bounding box
BoundingBox bounding_box_empty(void);

// Expand bounding box to include point
void bounding_box_expand(BoundingBox *box, Vec3 point);

// Merge two bounding boxes
void bounding_box_merge(BoundingBox *dest, const BoundingBox *src);

// Compute bounding box for mesh
void mesh_compute_bounds(Mesh *mesh);

// === INTERSECTION FUNCTIONS ===
// Ray-triangle intersection (reusable)
bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat);

#endif // TRIANGLE_MESH_H
