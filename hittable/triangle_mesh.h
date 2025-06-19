#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable/hit_record.h"
#include "hittable/hittable.h"
#include "material/material.h"
#include "triangle_raw.h"
#include <stdbool.h>

typedef struct MeshLoader {
  Material *default_material; // Default material for all triangles
  DynArray *materials;        // Array of materials (one per triangle, optional)
  bool per_triangle_materials; // Whether to use per-triangle materials
} MeshLoader;

// Create a mesh loader with a default material
MeshLoader *mesh_loader_create(Material *default_material);

// Add per-triangle material support
void mesh_loader_enable_per_triangle_materials(MeshLoader *loader);
void mesh_loader_add_material(MeshLoader *loader, Material *material);

// Add triangle and convert to hittable - adds to the provided hittable list
void mesh_loader_add_triangle(MeshLoader *loader, Hittable *hittable_list,
                              Vec3 v0, Vec3 v1, Vec3 v2);

// Add triangle with specific material
void mesh_loader_add_triangle_with_material(MeshLoader *loader,
                                            Hittable *hittable_list, Vec3 v0,
                                            Vec3 v1, Vec3 v2,
                                            Material *material);

// Clean up the loader (doesn't affect the created hittables)
void mesh_loader_destroy(MeshLoader *loader);

// Load OBJ file and add all triangles as individual hittables
typedef struct MeshLoadResult {
  bool success;
  char error_message[256];
  int triangle_count;
} MeshLoadResult;

// Load OBJ and add triangles to hittable list
MeshLoadResult mesh_load_obj(const char *filename, MeshLoader *loader,
                             Hittable *hittable_list, Vec3 scale,
                             Vec3 translation, Vec3 rotation);

// Ray-triangle intersection (reusable)
bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat);

#endif // TRIANGLE_MESH_H