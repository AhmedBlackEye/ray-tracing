#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable.h"
#include "material/material.h"
#include "triangle_hittable.h"
#include "triangle_raw.h"

// Möller-Trumbore ray-triangle intersection algorithm
static bool triangle_hit(Hittable *self, Ray r, Interval t_bounds,
                         HitRecord *rec) {
  const double EPSILON = 1e-13;
  TriangleRaw *tri = self->data;
  // Calculate determinant
  Vec3 h = vec3_cross(r.direction, tri->edge2);
  double det = vec3_dot(tri->edge1, h);

  if (fabs(det) < EPSILON) {
    // printf("Fail parallel \n");
    return false; // Ray is parallel to triangle
  }
  // Calculate barycentric coordinate u
  double inv = 1.0 / det;
  Vec3 s = vec3_sub(r.origin, tri->v0);
  double u = inv * vec3_dot(s, h);

  if (u < 0.0 || u > 1.0) {
    // printf("Fail 1");
    return false;
  }

  // Calculate barycentric coordinate v
  Vec3 q = vec3_cross(s, tri->edge1);
  double v = inv * vec3_dot(r.direction, q);

  if (v < 0.0 || u + v > 1.0) {
    // printf("Fail 2");
    return false;
  }
  // Calculate t parameter
  double t = inv * vec3_dot(tri->edge2, q);

  // printf("\nt value: %f", t);
  if (interval_surrounds(t_bounds, t)) {
    rec->t = t;
    rec->p = ray_at(r, t);

    // Ensure normal faces outward from ray
    hitrec_set_face_normal(rec, r, tri->normal);
    // printf("Triangle hit at t=%.2f, p=(%.2f, %.2f, %.2f)\n", t,
    //        rec->p.x, rec->p.y, rec->p.z);

    return true;
  }

  return false;
}

static void triangle_hittable_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_TRIANGLE) {
    printf("TriangleHittable: Invalid or NULL\n");
    return;
  }

  TriangleHittable *tri_hit = (TriangleHittable *)hittable->data;
  triangle_print_raw(&tri_hit->triangle);
}

Hittable *triangle_hittable_create(Vec3 v0, Vec3 v1, Vec3 v2, Material mat) {
  TriangleHittable *tri_hit_data = malloc(sizeof(TriangleHittable));
  assert(tri_hit_data != NULL);

  tri_hit_data->triangle = triangle_raw_create(v0, v1, v2, mat);
  tri_hit_data->material = mat;

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = tri_hit_data;
  hittable->destroy = (HittableDestroyFn)triangle_destroy;
  hittable->hit = (HitFn)triangle_hit;

  return hittable;
}
