#include "triangle_mesh.h"
#include "core/dyn_array.h"
#include "core/vec3.h"
#include "triangle.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static bool ray_box_intersect(Ray ray, BoundingBox box, Interval t_bounds) {
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

    if (fabs(ray_dir) < 1e-8) {
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

    double t_min = fmax(t0, t_bounds.min);
    double t_max = fmin(t1, t_bounds.max);

    if (t_min > t_max)
      return false;

    t_bounds.min = t_min;
    t_bounds.max = t_max;
  }
  return true;
}

static bool triangle_mesh_hit(const TriangleMesh *mesh, Ray r,
                              Interval t_bounds, HitRecord *rec) {
  // STEP 1: Quick bounding box rejection
  if (!ray_box_intersect(r, mesh->bounds, t_bounds)) {
    return false; // Ray misses entire mesh - skip ALL triangles!
  }

  // STEP 2: Test individual triangles (same as your original)
  bool hit_anything = false;
  double closest_so_far = t_bounds.max;
  HitRecord temp_rec;

  for (int i = 0; i < mesh->triangle_count; i++) {
    Triangle *tri = (Triangle *)dynarray_get(mesh->triangles, i);
    Interval triangle_bounds = interval_make(t_bounds.min, closest_so_far);

    if (triangle_hit(tri, r, triangle_bounds, &temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      if (rec)
        *rec = temp_rec;
    }
  }

  return hit_anything;
}

void triangle_mesh_destroy(void *self) {
  assert(self != NULL);
  TriangleMesh *mesh = (TriangleMesh *)self;
  Hittable *hittable = (Hittable *)self;
  dynarray_destroy(mesh->triangles);
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

// Utility function for when creating meshes from objects.
// BoundingBox compute_mesh_bounds(const TriangleMesh *mesh) {
//   if (mesh->triangle_count == 0) {
//     return (BoundingBox){{0, 0, 0}, {0, 0, 0}};
//   }

//   Triangle *first_tri = (Triangle *)dynarray_get(mesh->triangles, 0);
//   BoundingBox bounds = {first_tri->v0, first_tri->v0};

//   for (int i = 0; i < mesh->triangle_count; i++) {
//     Triangle *tri = (Triangle *)dynarray_get(mesh->triangles, i);
//     Vec3 vertices[3] = {tri->v0, tri->v1, tri->v2};

//     for (int v = 0; v < 3; v++) {
//       bounds.min.x = MIN(bounds.min.x, vertices[v].x);
//       bounds.min.y = MIN(bounds.min.y, vertices[v].y);
//       bounds.min.z = MIN(bounds.min.z, vertices[v].z);

//       bounds.max.x = MAX(bounds.max.x, vertices[v].x);
//       bounds.max.y = MAX(bounds.max.y, vertices[v].y);
//       bounds.max.z = MAX(bounds.max.z, vertices[v].z);
//     }
//   }

//   return bounds;
// }

void mesh_print(const TriangleMesh *mesh) {
  if (mesh == NULL) {
    printf("Mesh is null.\n");
  }
  printf("TriangleMesh\n");
  printf("Number of triangles: %d", mesh->triangle_count);
  if (mesh->triangle_count > 0) {
    Vec3 size = vec3_sub(mesh->bounds.max, mesh->bounds.min);
    printf(", bounds: %.2f%.2f%.2f.", size.x, size.y, size.z);
  }
}

Hittable *triangle_mesh_create(void) {
  TriangleMesh *mesh = malloc(sizeof(TriangleMesh));
  assert(mesh != NULL);

  Hittable *hittable = malloc(sizeof(Hittable));
  assert(hittable != NULL);

  mesh->triangles =
      dynarray_create(MESH_INITIAL_CAPACITY, (GPrintFn)hittable_print,
                      (GDestroyFn)hittable_destroy);
  mesh->bounds.max = (Vec3){INFINITY, INFINITY, INFINITY};
  mesh->bounds.min = (Vec3){-INFINITY, -INFINITY, -INFINITY};

  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = mesh;
  hittable->hit = triangle_mesh_hit;
  hittable->destroy = (HittableDestroyFn)triangle_mesh_destroy;

  return hittable;
}