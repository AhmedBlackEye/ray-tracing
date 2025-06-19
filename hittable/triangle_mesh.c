#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "hittable_list.h"
#include "triangle_hittable.h"
#include "triangle_mesh.h"
#include "triangle_raw.h"

#define EPSILON 1e-13
#define MATERIAL_INI_CAPACITY 10

// === MESH LOADER IMPLEMENTATION ===

static void dummy_material_destroy(void *material) {
  // Materials are owned by Scene, not MeshLoader - do nothing
  (void)material; // Suppress unused warning
}

MeshLoader *mesh_loader_create(Material *default_material) {
  MeshLoader *loader = malloc(sizeof(MeshLoader));
  assert(loader != NULL);

  loader->default_material = default_material;
  loader->materials =
      dynarray_create(MATERIAL_INI_CAPACITY, NULL, dummy_material_destroy);
  loader->per_triangle_materials = false;

  return loader;
}

void mesh_loader_enable_per_triangle_materials(MeshLoader *loader) {
  assert(loader != NULL);
  loader->per_triangle_materials = true;
}

void mesh_loader_add_material(MeshLoader *loader, Material *material) {
  assert(loader != NULL);
  assert(material != NULL);
  dynarray_push(loader->materials, material);
}

void mesh_loader_add_triangle(MeshLoader *loader, Hittable *hittable_list,
                              Vec3 v0, Vec3 v1, Vec3 v2) {
  assert(loader != NULL);
  assert(hittable_list != NULL);
  assert(hittable_list->type == HITTABLE_LIST);

  Material *material = loader->default_material;

  // If using per-triangle materials, get the next material
  if (loader->per_triangle_materials) {
    int triangle_count = dynarray_size(((DynArray *)hittable_list->data));
    if (triangle_count < dynarray_size(loader->materials)) {
      material = (Material *)dynarray_get(loader->materials, triangle_count);
    }
    // If we run out of materials, fall back to default
  }

  // Create individual hittable triangle
  Hittable *triangle = triangle_hittable_create(v0, v1, v2, material);
  hittablelist_add(hittable_list, triangle);
}

void mesh_loader_add_triangle_with_material(MeshLoader *loader,
                                            Hittable *hittable_list, Vec3 v0,
                                            Vec3 v1, Vec3 v2,
                                            Material *material) {
  assert(loader != NULL);
  assert(hittable_list != NULL);
  assert(hittable_list->type == HITTABLE_LIST);
  assert(material != NULL);

  // Create individual hittable triangle with specific material
  Hittable *triangle = triangle_hittable_create(v0, v1, v2, material);
  hittablelist_add(hittable_list, triangle);
}

void mesh_loader_destroy(MeshLoader *loader) {
  if (loader) {
    dynarray_destroy(loader->materials);
    free(loader);
  }
}

bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat) {

  Vec3 h = vec3_cross(r.direction, tri->edge2);
  double det = vec3_dot(tri->edge1, h);

  if (fabs(det) < EPSILON) {
    return false; // Ray is parallel to triangle
  }

  // Calculate barycentric coordinate u
  double inv = 1.0 / det;
  Vec3 s = vec3_sub(r.origin, tri->v0);
  double u = inv * vec3_dot(s, h);

  if (u < 0.0 || u > 1.0) {
    return false;
  }

  // Calculate barycentric coordinate v
  Vec3 q = vec3_cross(s, tri->edge1);
  double v = inv * vec3_dot(r.direction, q);

  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  // Calculate t parameter
  double t = inv * vec3_dot(tri->edge2, q);

  if (interval_surrounds(t_bounds, t)) {
    rec->t = t;
    rec->p = ray_at(r, t);
    hitrec_set_face_normal(rec, r, tri->normal);
    rec->mat = mat;
    return true;
  }

  return false;
}