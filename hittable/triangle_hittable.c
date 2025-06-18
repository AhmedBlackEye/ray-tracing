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
  TriangleHittable *tri_hit_data = malloc(sizeof(struct TriangleHittable));
  assert(tri_hit_data != NULL);

  tri_hit_data->triangle = triangle_raw_create(v0, v1, v2);

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = tri_hit_data;
  hittable->mat = mat;
  hittable->destroy = (HittableDestroyFn)triangle_destroy;
  hittable->hit = (HitFn)triangle_hit;

  return hittable;
}
