#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable.h"
#include "triangle_hittable.h"
#include "triangle_raw.h"

Hittable *triangle_create_hittable(Vec3 v0, Vec3 v1, Vec3 v2) {
  TriangleHittable *tri_hit_data = malloc(sizeof(TriangleHittable));
  assert(tri_hit != NULL);

  tri_hit_data->triangle = triangle_create_raw(v0, v1, v2);

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = tri_hit_data;
  hittable->destroy = (HittableDestroyFn)triangle_destroy;
  hittable->hit = (HitFn)triangle_hit;

  return hittable;
}

static void triangle_hittable_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_TRIANGLE) {
    printf("TriangleHittable: Invalid or NULL\n");
    return;
  }

  TriangleHittable *tri_hit = (TriangleHittable *)hittable->data;
  triangle_print_raw(&tri_hit->triangle);
}
