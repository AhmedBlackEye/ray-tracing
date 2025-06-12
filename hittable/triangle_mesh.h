#include "../core/dyn_array.h"
#include "../core/interval.h"
#include "../core/ray.h"
#include "../core/vec3.h"
#include "hittable.h"

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH

#define MESH_INITIAL_CAPACITY 10

typedef struct BoundingBox {
  Vec3 min;
  Vec3 max;
} BoundingBox;

typedef struct TriangleMesh {
  DynArray *triangles; // Array of Triangle* objects
  BoundingBox bounds;
  int triangle_count;
} TriangleMesh;

bool ray_box_intersect(Ray ray, BoundingBox box, Interval t_bounds) {
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

#endif // TRIANGLE_MESH_h