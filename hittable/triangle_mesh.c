#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/dyn_array.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "interval.h"
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

    t_bounds->min = t_min;
    t_bounds->max = t_max;
  }
  return true;
}
static bool triangle_mesh_hit(const Hittable *mesh_hittable, Ray r,
                              Interval t_bounds, HitRecord *rec) {
  assert(mesh_hittable != NULL);
  assert(mesh_hittable->type == HITTABLE_TRIANGLE_MESH);

  TriangleMesh *mesh = (TriangleMesh *)mesh_hittable->data;

  // Check if ray and box interesect
  if (!ray_box_intersect(r, mesh->bounds, &t_bounds)) {
    return false;
  }

  bool hit_anything = false;
  double closest_so_far = t_bounds.max;
  HitRecord temp_rec;

  for (int i = 0; i < mesh->triangle_count; i++) {
    TriangleRaw *tri = (TriangleRaw *)dynarray_get(mesh->triangles, i);
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
void triangle_mesh_add_triangle(TriangleMesh *mesh, Vec3 v0, Vec3 v1, Vec3 v2,
                                Material mat) {
  TriangleRaw *tri = triangle_raw_create(v0, v1, v2, mat);
  dynarray_push(mesh->raw_triangles, tri);
  mesh->triangle_count++;

  // Update bounding box
  update_bounding_box(&mesh->bounds, v0);
  update_bounding_box(&mesh->bounds, v1);
  update_bounding_box(&mesh->bounds, v2);
}

static void update_bounding_box(BoundingBox *box, Vec3 point) {
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

void triangle_mesh_print(const TriangleMesh *mesh) {
  if (mesh == NULL) {
    printf("TriangleMesh: NULL\n");
    return;
  }

  printf("TriangleMesh Container {\n");
  printf("Triangle count: %d\n", triangle_mesh_size(mesh));
  printf("Memory efficient storage of raw triangles\n");
  printf("}\n");
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

/**
 * What to do:
 * Triangle_mesh currently holds individual hittable triangle objects
 * (meaning they all have their own destry function and hit function which
 * are the same) We want to abstract these functions out so that we only
 * store the Triangle object and not a Triangle Hittable object. When we
 * want to destroy or check if hit, we just call those functions to that
 * triangle.
 */