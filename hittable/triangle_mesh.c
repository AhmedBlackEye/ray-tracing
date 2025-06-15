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

/**
 * This creates the triangle but only for mesh, instead of creating hittables,
 * we create just triangles Meaning: We leave the triangle destroy and triangle
 * hit functions to the triangles we have made but we create the Triangle object
 * The Triangle object does not have a destroy ot hit, instead we call these
 * functions to the actual functions we have declared for the triangles to make
 * the 3D object.
 */
// Use when making triangle_mesh a hittable. (if we want to)
// static bool ray_box_intersect(Ray ray, BoundingBox box, Interval *t_bounds) {
//   for (int axis = 0; axis < 3; axis++) {
//     double ray_origin, ray_dir, box_min, box_max;

//     if (axis == 0) {
//       ray_origin = ray.origin.x;
//       ray_dir = ray.direction.x;
//       box_min = box.min.x;
//       box_max = box.max.x;
//     } else if (axis == 1) {
//       ray_origin = ray.origin.y;
//       ray_dir = ray.direction.y;
//       box_min = box.min.y;
//       box_max = box.max.y;
//     } else {
//       ray_origin = ray.origin.z;
//       ray_dir = ray.direction.z;
//       box_min = box.min.z;
//       box_max = box.max.z;
//     }

//     if (fabs(ray_dir) < 1e-8) {
//       if (ray_origin < box_min || ray_origin > box_max)
//         return false;
//       continue;
//     }

//     double inv_dir = 1.0 / ray_dir;
//     double t0 = (box_min - ray_origin) * inv_dir;
//     double t1 = (box_max - ray_origin) * inv_dir;

//     if (t0 > t1) {
//       double temp = t0;
//       t0 = t1;
//       t1 = temp;
//     }

//     double t_min = fmax(t0, t_bounds->min);
//     double t_max = fmin(t1, t_bounds->max);

//     if (t_min > t_max)
//       return false;

//     t_bounds->min = t_min;
//     t_bounds->max = t_max;
//   }
//   return true;
// }
TriangleMesh *triangle_mesh_create(void) {
  TriangleMesh *mesh = malloc(sizeof(TriangleMesh));
  assert(mesh != NULL);

  // Store raw triangles efficiently
  mesh->raw_triangles = dynarray_create(INIT_CAP, (GPrintFn)triangle_raw_print,
                                        (GDestroyFn)triangle_destroy);

  return mesh;
}

// Add a single NEW triangle to the mesh via passing in vectors and material for
// the triangle.
void triangle_mesh_add_triangle(TriangleMesh *mesh, Vec3 v0, Vec3 v1, Vec3 v2) {
  TriangleRaw *tri = triangle_raw_create(v0, v1, v2);
  dynarray_push(mesh->raw_triangles, tri);

  // Update bounding box
  // update_bounding_box(&mesh->bounds, v0);
  // update_bounding_box(&mesh->bounds, v1);
  // update_bounding_box(&mesh->bounds, v2);
}

// static void update_bounding_box(BoundingBox *box, Vec3 point) {
//   box->min.x = fmin(box->min.x, point.x);
//   box->min.y = fmin(box->min.y, point.y);
//   box->min.z = fmin(box->min.z, point.z);

//   box->max.x = fmax(box->max.x, point.x);
//   box->max.y = fmax(box->max.y, point.y);
//   box->max.z = fmax(box->max.z, point.z);
// }

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

  printf("TriangleMesh Container {\n");
  printf("Triangle count: %d\n", triangle_mesh_size(mesh));
  printf("Memory efficient storage of raw triangles\n");
  printf("}\n");
}
