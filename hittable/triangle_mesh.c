#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "triangle_hittable.h"
#include "triangle_mesh.h"
#include "triangle_raw.h"

// Forward declarations for static functions
static bool mesh_hit(const Hittable *self, Ray r, Interval t_bounds,
                     HitRecord *rec);
static void mesh_destroy(void *self);
static bool ray_box_intersect(Ray ray, BoundingBox box, Interval *t_bounds);

// === MESH DATA FUNCTIONS ===

Mesh *mesh_create_data(Material *mat) { // RENAMED: from mesh_create
  Mesh *mesh = malloc(sizeof(Mesh));
  assert(mesh != NULL);

  mesh->raw_triangles = dynarray_create(INIT_CAP, (GPrintFn)triangle_raw_print,
                                        (GDestroyFn)triangle_destroy);
  mesh->bounds_dirty = false;
  mesh->bounds = bounding_box_empty();
  mesh->material = mat;
  return mesh;
}

void mesh_add_triangle(Mesh *mesh, Vec3 v0, Vec3 v1, Vec3 v2) {
  TriangleRaw *tri = triangle_raw_create(v0, v1, v2);
  dynarray_push(mesh->raw_triangles, tri);
  mesh->bounds_dirty = true;
}

void mesh_destroy_data(Mesh *mesh) { // NEW: Clean up mesh data
  assert(mesh != NULL);
  dynarray_destroy(mesh->raw_triangles);
  free(mesh);
}

void mesh_print_data(const Mesh *mesh) { // NEW: Print mesh data directly
  if (mesh == NULL) {
    printf("Mesh: NULL\n");
    return;
  }

  printf("Mesh {\n");
  printf("  Triangle count: %d\n", dynarray_size(mesh->raw_triangles));

  if (mesh->bounds.valid) {
    printf("  Bounding box: (%.3f,%.3f,%.3f) to (%.3f,%.3f,%.3f)\n",
           mesh->bounds.min.x, mesh->bounds.min.y, mesh->bounds.min.z,
           mesh->bounds.max.x, mesh->bounds.max.y, mesh->bounds.max.z);
  } else {
    printf("  Bounding box: not computed\n");
  }
  printf("}\n");
}

// === HITTABLE WRAPPER FUNCTIONS ===

Hittable *mesh_create_hittable(Mesh *mesh,
                               Material *mat) { // RENAMED: from mesh_create
  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_TRIANGLE_MESH;
  hittable->data = mesh;
  hittable->destroy = mesh_destroy;
  hittable->hit = mesh_hit;
  hittable->mat = mat;

  return hittable;
}

void mesh_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_TRIANGLE_MESH) {
    printf("MeshHittable: Invalid or NULL\n");
    return;
  }

  const Mesh *mesh = (const Mesh *)hittable->data;
  printf("MeshHittable {\n");
  printf("  Triangle count: %d\n", dynarray_size(mesh->raw_triangles));

  if (mesh->bounds.valid) {
    printf("  Bounding box: (%.3f,%.3f,%.3f) to (%.3f,%.3f,%.3f)\n",
           mesh->bounds.min.x, mesh->bounds.min.y, mesh->bounds.min.z,
           mesh->bounds.max.x, mesh->bounds.max.y, mesh->bounds.max.z);
  } else {
    printf("  Bounding box: not computed\n");
  }
  printf("}\n");
}

// === BOUNDING BOX FUNCTIONS ===

BoundingBox bounding_box_empty(void) {
  return (BoundingBox){.min = {INFINITY, INFINITY, INFINITY},
                       .max = {-INFINITY, -INFINITY, -INFINITY},
                       .valid = false};
}

void bounding_box_expand(BoundingBox *box, Vec3 point) {
  if (!box->valid) {
    box->min = point;
    box->max = point;
    box->valid = true;
  } else {
    box->min.x = fmin(box->min.x, point.x);
    box->min.y = fmin(box->min.y, point.y);
    box->min.z = fmin(box->min.z, point.z);

    box->max.x = fmax(box->max.x, point.x);
    box->max.y = fmax(box->max.y, point.y);
    box->max.z = fmax(box->max.z, point.z);
  }
}

void bounding_box_merge(BoundingBox *dest, const BoundingBox *src) {
  if (!src->valid)
    return;

  if (!dest->valid) {
    *dest = *src;
  } else {
    dest->min.x = fmin(dest->min.x, src->min.x);
    dest->min.y = fmin(dest->min.y, src->min.y);
    dest->min.z = fmin(dest->min.z, src->min.z);

    dest->max.x = fmax(dest->max.x, src->max.x);
    dest->max.y = fmax(dest->max.y, src->max.y);
    dest->max.z = fmax(dest->max.z, src->max.z);
  }
}

void mesh_compute_bounds(Mesh *mesh) {
  mesh->bounds = bounding_box_empty();

  for (int i = 0; i < dynarray_size(mesh->raw_triangles); i++) {
    TriangleRaw *tri = (TriangleRaw *)dynarray_get(mesh->raw_triangles, i);
    bounding_box_expand(&mesh->bounds, tri->v0);
    bounding_box_expand(&mesh->bounds, tri->v1);
    bounding_box_expand(&mesh->bounds, tri->v2);
  }

  mesh->bounds_dirty = false;
}

// === INTERSECTION FUNCTIONS ===

bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat) {
  // YOUR EXACT Möller-Trumbore code
  Vec3 h = vec3_cross(r.direction, tri->edge2);
  double det = vec3_dot(tri->edge1, h);

  if (fabs(det) < TRIANGLE_MESH_EPSILON) {
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
    // Fill the hit record
    rec->t = t;
    rec->p = ray_at(r, t);
    hitrec_set_face_normal(rec, r, tri->normal);
    rec->mat = mat;
    return true;
  }

  return false;
}

// === STATIC HELPER FUNCTIONS ===

static bool ray_box_intersect(Ray ray, BoundingBox box, Interval *t_bounds) {
  for (int axis = 0; axis < 3; axis++) {
    double ray_origin, ray_dir, box_min, box_max;

    if (axis == 0) {
      ray_origin = ray.origin.x;
      ray_dir = ray.direction.x;
      box_min = box.min.x;
      box_max = box.max.x;
    } else if (axis == 1) {
      ray_origin = ray.origin.y;
      ray_dir = ray.direction.y;
      box_min = box.min.y;
      box_max = box.max.y;
    } else {
      ray_origin = ray.origin.z;
      ray_dir = ray.direction.z;
      box_min = box.min.z;
      box_max = box.max.z;
    }

    if (fabs(ray_dir) < TRIANGLE_MESH_EPSILON) {
      if (ray_origin < box_min || ray_origin > box_max)
        return false;
      continue;
    }

    double inv_dir = 1.0 / ray_dir;
    double t0 = (box_min - ray_origin) * inv_dir;
    double t1 = (box_max - ray_origin) * inv_dir;

    if (t0 > t1) {
      double temp = t0;
      t0 = t1;
      t1 = temp;
    }

    double t_min = fmax(t0, t_bounds->min);
    double t_max = fmin(t1, t_bounds->max);

    if (t_min > t_max)
      return false;

    t_bounds->min = t_min;
    t_bounds->max = t_max;
  }
  return true;
}

static void mesh_destroy(void *self) {
  assert(self != NULL);
  free(self);
}

static bool mesh_hit(const Hittable *self, Ray r, Interval t_bounds,
                     HitRecord *rec) {
  assert(self != NULL);
  assert(self->type == HITTABLE_TRIANGLE_MESH);

  Mesh *mesh = (Mesh *)self->data;

  if (mesh->bounds_dirty) {
    mesh_compute_bounds(mesh);
  }

  Interval box_t = t_bounds;
  if (!ray_box_intersect(r, mesh->bounds, &box_t)) {
    return false;
  }

  bool hit_anything = false;
  HitRecord temp_rec;
  double closest_so_far = t_bounds.max;

  for (int i = 0; i < dynarray_size(mesh->raw_triangles); i++) {
    TriangleRaw *tri = (TriangleRaw *)dynarray_get(mesh->raw_triangles, i);
    Interval tri_bounds = {t_bounds.min, closest_so_far};

    if (triangle_raw_intersect(tri, r, tri_bounds, &temp_rec, mesh->material)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }

  return hit_anything;
}
