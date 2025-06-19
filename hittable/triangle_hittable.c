#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "material/material.h"
#include "triangle_hittable.h"
#include "triangle_raw.h"

void triangle_hittable_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_TRIANGLE) {
    printf("TriangleHittable: Invalid or NULL\n");
    return;
  }

  TriangleHittable *tri_hit = (TriangleHittable *)hittable->data;
  triangle_raw_print(&tri_hit->triangle);
}

Hittable *triangle_hittable_create(Vec3 v0, Vec3 v1, Vec3 v2, Material *mat) {
  // Validate inputs first
  assert(mat != NULL);

  // Create the triangle data
  TriangleHittable *tri_hit_data = malloc(sizeof(TriangleHittable));
  assert(tri_hit_data != NULL);

  // Initialize the triangle data
  tri_hit_data->triangle = triangle_raw_create(v0, v1, v2);

  // Create the hittable wrapper
  Hittable *hittable = malloc(sizeof(Hittable));
  assert(hittable != NULL);

  // Initialize ALL fields explicitly (this might have been the issue)
  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = tri_hit_data;
  hittable->mat = mat;
  hittable->destroy = triangle_destroy;
  hittable->hit = triangle_hit;

  // Initialize bbox to empty first (defensive programming)
  hittable->bbox = aabb_empty();

  // Calculate bounding box with explicit temporary variables
  // (prevents potential compiler optimization issues)
  double min_x = fmin(fmin(v0.x, v1.x), v2.x);
  double min_y = fmin(fmin(v0.y, v1.y), v2.y);
  double min_z = fmin(fmin(v0.z, v1.z), v2.z);

  double max_x = fmax(fmax(v0.x, v1.x), v2.x);
  double max_y = fmax(fmax(v0.y, v1.y), v2.y);
  double max_z = fmax(fmax(v0.z, v1.z), v2.z);

  // Create points with explicit assignment
  Vec3 min_point;
  min_point.x = min_x;
  min_point.y = min_y;
  min_point.z = min_z;

  Vec3 max_point;
  max_point.x = max_x;
  max_point.y = max_y;
  max_point.z = max_z;

  // Add padding so BVH doesnt break (div by zero)
  const double padding_amount = 0.001; // 1mm
  Vec3 padding = {padding_amount, padding_amount, padding_amount};

  min_point = vec3_sub(min_point, padding);
  max_point = vec3_add(max_point, padding);

  // Create AABB with explicit assignment
  hittable->bbox = aabb_from_points(min_point, max_point);

  // Validate the created bounding box (defensive check)
  double size_x = hittable->bbox.x.max - hittable->bbox.x.min;
  double size_y = hittable->bbox.y.max - hittable->bbox.y.min;
  double size_z = hittable->bbox.z.max - hittable->bbox.z.min;

  return hittable;
}
