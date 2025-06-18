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

#define EPSILON 1e-13

// Forward declarations for static functions
static bool mesh_hit(const Hittable *self, Ray r, Interval t_bounds,
                     HitRecord *rec);
static void mesh_destroy(void *self);
static bool ray_box_intersect(Ray ray, BoundingBox box, Interval *t_bounds);
static BoundingBox bounding_box_empty(void);
void mesh_compute_bounds_and_update_hittable(Hittable *mesh_hittable);
static void bounding_box_expand(BoundingBox *box, Vec3 point);
static TriangleArr *triarray_create(int initial_capacity);
static void triarray_push(TriangleArr *arr, TriangleRaw tri);
static void triarray_destroy(TriangleArr *arr);
static TriangleRaw *triarray_get(TriangleArr *arr, int index);

// === CREATING HITTABLE MESH ===

Hittable *mesh_create(Material *mat) {
  // Create mesh data
  Mesh *mesh = malloc(sizeof(Mesh));
  assert(mesh != NULL);

  mesh->triangles = triarray_create(INITIAL_CAPACITY);

  mesh->bounds_dirty = false;
  mesh->bounds = bounding_box_empty();
  mesh->material = mat;

  // Create hittable wrapper
  Hittable *hittable = malloc(sizeof(Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_TRIANGLE_MESH;
  hittable->data = mesh;
  hittable->destroy = (HittableDestroyFn)mesh_destroy;
  hittable->hit = (HitFn)mesh_hit;
  hittable->mat = mat;
  hittable->bbox = aabb_empty();

  return hittable;
}

void mesh_add_triangle(Hittable *mesh_hittable, Vec3 v0, Vec3 v1, Vec3 v2) {
  assert(mesh_hittable != NULL);
  assert(mesh_hittable->type == HITTABLE_TRIANGLE_MESH);

  Mesh *mesh = (Mesh *)mesh_hittable->data;
  TriangleRaw tri = triangle_raw_create(v0, v1, v2);

  triarray_push(mesh->triangles, tri);
  mesh->bounds_dirty = true;

  mesh_compute_bounds_and_update_hittable(mesh_hittable);
}

void mesh_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_TRIANGLE_MESH) {
    printf("TriangleMesh: Invalid or NULL\n");
    return;
  }

  const Mesh *mesh = (const Mesh *)hittable->data;
  printf("TriangleMesh {\n");
  printf("  Triangle count: %d\n", mesh->triangles->size);

  if (mesh->bounds.valid) {
    printf("  Bounding box: (%.3f,%.3f,%.3f) to (%.3f,%.3f,%.3f)\n",
           mesh->bounds.min.x, mesh->bounds.min.y, mesh->bounds.min.z,
           mesh->bounds.max.x, mesh->bounds.max.y, mesh->bounds.max.z);
  } else {
    printf("  Bounding box: not computed\n");
  }
  printf("}\n");
}

bool triangle_raw_intersect(const TriangleRaw *tri, Ray r, Interval t_bounds,
                            HitRecord *rec, Material *mat) {
  // YOUR EXACT Möller-Trumbore code
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
    // Fill the hit record
    rec->t = t;
    rec->p = ray_at(r, t);
    hitrec_set_face_normal(rec, r, tri->normal);
    rec->mat = mat;
    return true;
  }

  return false;
}

// === HELPER FUNCTIONS ===

static BoundingBox bounding_box_empty(void) {
  return (BoundingBox){.min = {INFINITY, INFINITY, INFINITY},
                       .max = {-INFINITY, -INFINITY, -INFINITY},
                       .valid = false};
}

static void bounding_box_expand(BoundingBox *box, Vec3 point) {
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

void mesh_compute_bounds_and_update_hittable(Hittable *mesh_hittable) {
  assert(mesh_hittable->type == HITTABLE_TRIANGLE_MESH);
  Mesh *mesh = (Mesh *)mesh_hittable->data;

  if (!mesh->bounds_dirty)
    return;

  // Compute mesh bounds
  mesh->bounds = bounding_box_empty();

  for (int i = 0; i < mesh->triangles->size; i++) {
    TriangleRaw *tri = triarray_get(mesh->triangles, i);
    bounding_box_expand(&mesh->bounds, tri->v0);
    bounding_box_expand(&mesh->bounds, tri->v1);
    bounding_box_expand(&mesh->bounds, tri->v2);
  }

  mesh->bounds_dirty = false;

  // Update the hittable's bounding box for BVH
  if (mesh->bounds.valid) {
    mesh_hittable->bbox = aabb_from_points(mesh->bounds.min, mesh->bounds.max);
  } else {
    printf("WARNING: Mesh bounds are invalid!\n");
    mesh_hittable->bbox = aabb_empty();
  }
}

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

    if (fabs(ray_dir) < EPSILON) {
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
  Hittable *hittable = (Hittable *)self;
  Mesh *mesh = (Mesh *)hittable->data;

  // Clean up mesh data
  if (mesh) {
    triarray_destroy(mesh->triangles);
    free(mesh);
  }

  // Free the hittable
  free(self);
}

static bool mesh_hit(const Hittable *self, Ray r, Interval t_bounds,
                     HitRecord *rec) {
  assert(self != NULL);
  assert(self->type == HITTABLE_TRIANGLE_MESH);

  Mesh *mesh = (Mesh *)self->data;

  if (mesh->bounds_dirty) {
    mesh_compute_bounds_and_update_hittable((Hittable *)self);
  }

  Interval box_t = t_bounds;
  if (!ray_box_intersect(r, mesh->bounds, &box_t)) {
    return false;
  }

  bool hit_anything = false;
  HitRecord temp_rec;
  double closest_so_far = t_bounds.max;

  for (int i = 0; i < mesh->triangles->size; i++) {
    TriangleRaw *tri = triarray_get(mesh->triangles, i);
    Interval tri_bounds = {t_bounds.min, closest_so_far};

    if (triangle_raw_intersect(tri, r, tri_bounds, &temp_rec, mesh->material)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }

  return hit_anything;
}

// === TRIANGLE ARRAY FUNCTIONS ===

TriangleArr *triarray_create(int initial_capacity) {
  assert(initial_capacity > 0);
  TriangleArr *new = malloc(sizeof(TriangleArr));
  assert(new);
  new->capacity = initial_capacity;
  new->data = malloc(sizeof(TriangleRaw) * initial_capacity);
  new->size = 0;
  return new;
}

void triarray_push(TriangleArr *arr, TriangleRaw tri) {
  assert(arr != NULL);
  if (arr->size >= arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->data = realloc(arr->data, arr->capacity * sizeof(struct TriangleRaw));
    assert(arr->data);
  }
  arr->data[arr->size++] = tri;
}

void triarray_destroy(TriangleArr *arr) {
  assert(arr != NULL);
  free(arr->data);
  free(arr);
}

TriangleRaw *triarray_get(TriangleArr *arr, int index) {
  assert(arr != NULL);
  assert(index >= 0 && index < arr->size);
  return &arr->data[index];
}

// void triarray_print(TriangleArr *arr) {
//   if (arr == NULL) {
//     printf("Triangle Array: NULL.\n");
//     return;
//   }
//   if (arr->size == 0) {
//     puts("[]");
//     return;
//   }
//   putchar('[');
//   for (int i = 0; i < arr->size; i++) {
//     triangle_raw_print(&arr->data[i]);
//     if (i < arr->size - 1) {
//       printf(", ");
//     }
//   }
//   puts("]");
// }

/**
 * Change triangle raw to triangle hittable
 * Use dynamic arrays
 * so no more triangle raw only triangle (and all are hittable)
 * mesh will store hittable triangles.
 * => can use for different materials.
 */