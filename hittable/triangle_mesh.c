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

TriangleMesh *triangle_mesh_create(void) {
  TriangleMesh *mesh = malloc(sizeof(TriangleMesh));
  assert(mesh != NULL);

  // Store raw triangles efficiently
  mesh->raw_triangles = dynarray_create(INIT_CAP, (GPrintFn)triangle_raw_print,
                                        (GDestroyFn)triangle_destroy);
  mesh->bounds_dirty = false;
  mesh->bounds = bounding_box_empty();
  return mesh;
}

// Add a single NEW triangle to the mesh via passing in vectors and material for
// the triangle.
void triangle_mesh_add_triangle(TriangleMesh *mesh, Vec3 v0, Vec3 v1, Vec3 v2) {
  TriangleRaw *tri = triangle_raw_create(v0, v1, v2);
  dynarray_push(mesh->raw_triangles, tri);
  mesh->bounds_dirty = true;
}

static void expand_bounding_box(BoundingBox *box, Vec3 point) {
  box->min.x = fmin(box->min.x, point.x);
  box->min.y = fmin(box->min.y, point.y);
  box->min.z = fmin(box->min.z, point.z);

  box->max.x = fmax(box->max.x, point.x);
  box->max.y = fmax(box->max.y, point.y);
  box->max.z = fmax(box->max.z, point.z);
}

void triangle_mesh_destroy(TriangleMesh *mesh) {
  assert(mesh != NULL);
  dynarray_destroy(mesh->raw_triangles);
  free(mesh);
}

static int triangle_mesh_size(TriangleMesh *self) {
  return dynarray_size(self->raw_triangles);
}

void triangle_mesh_print(TriangleMesh *mesh) {
  if (mesh == NULL) {
    printf("TriangleMesh: NULL\n");
    return;
  }

  printf("TriangleMesh {\n");
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

// Bounding box and ray intersection optimisation functions:

BoundingBox bounding_box_empty(void) {
  return (BoundingBox){.min = {INFINITY, INFINITY, INFINITY},
                       .max = {-INFINITY, -INFINITY, -INFINITY},
                       .valid = false};
}

// Merge two boxes
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
// Add a point to bounding box (expanding it)
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

// Compute bounding box for the entire mesh
void triangle_mesh_compute_bounds(TriangleMesh *mesh) {
  mesh->bounds = bounding_box_empty();

  for (int i = 0; i < dynarray_size(mesh->raw_triangles); i++) {
    TriangleRaw *tri = (TriangleRaw *)dynarray_get(mesh->raw_triangles, i);
    bounding_box_expand(&mesh->bounds, tri->v0);
    bounding_box_expand(&mesh->bounds, tri->v1);
    bounding_box_expand(&mesh->bounds, tri->v2);
  }

  mesh->bounds_dirty = false;
}

/**
 * ray_box_intersect() checks to see if for any of the axis (x,y,z) i.e: 0,1,2,
 * the ray intersects the bounding box. If there are any overlaps then this
 * means the ray intersects and the intersection is stored in t_bounds.
 */
static bool ray_box_intersect(Ray ray, BoundingBox box, Interval *t_bounds) {
  for (int axis = 0; axis < 3; axis++) {
    double ray_origin, ray_dir, box_min, box_max;
    // Check for each axis that
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

    // For this axis, the ray is parallel. Ray only hits box if ray origin lies
    // between box_min and box_max.
    if (fabs(ray_dir) < EPSILON) {
      if (ray_origin < box_min || ray_origin > box_max)
        return false;
      continue;
    }

    double inv_dir = 1.0 / ray_dir;
    // intersection distances of the ray with the two planes of the box on this
    // axis.
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
    // Intersection found so update new bounds.
    t_bounds->min = t_min;
    t_bounds->max = t_max;
  }
  return true;
}

// Hittable Triangle Mesh Wrapper functions
static bool triangle_mesh_hit(const Hittable *self, Ray r, Interval t_bounds,
                              HitRecord *rec) {
  assert(self != NULL);
  assert(self->type == HITTABLE_TRIANGLE_MESH);
}