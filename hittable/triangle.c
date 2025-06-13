#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"

typedef struct Triangle {
  Vec3 v0, v1, v2;   // vertices
  Vec3 normal;       // pre-computed normal
  Vec3 edge1, edge2; // pre-computed edges
  Plane plane;
} Triangle;

// Möller-Trumbore ray-triangle intersection algorithm
bool triangle_hit(Triangle *tri, Ray r, Interval t_bounds, HitRecord *rec) {
  const double EPSILON = 1e-8;

  Vec3 h = vec3_cross(r.direction, tri->edge2);
  double a = vec3_dot(tri->edge1, h);

  if (fabs(a) < EPSILON) {
    return false; // Ray is parallel to triangle
  }

  double f = 1.0 / a;
  Vec3 s = vec3_sub(r.origin, tri->v0);
  double u = f * vec3_dot(s, h);

  if (u < 0.0 || u > 1.0) {
    return false;
  }

  Vec3 q = vec3_cross(s, tri->edge1);
  double v = f * vec3_dot(r.direction, q);

  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  double t = f * vec3_dot(tri->edge2, q);

  if (interval_surrounds(t_bounds, t)) {
    rec->t = t;
    rec->p = ray_at(r, t);

    // Ensure normal faces outward from ray
    bool front_face = vec3_dot(r.direction, tri->normal) < 0;
    rec->normal = front_face ? tri->normal : vec3_scale(tri->normal, -1);
    rec->front_face = front_face;

    return true;
  }

  return false;
}

static void triangle_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

void triangle_print(const Hittable *hittable) {
  if (hittable == NULL) {
    printf("Triangle: NULL.\n");
    return;
  }
  Triangle *tri = (Triangle *)hittable;
  printf("Triangle v0:(%.2f,%.2f,%.2f), v1:(%.2f, %.2f, %.2f), "
         "v2:(%.2f,%.2f,%.2f)",
         tri->v0.x, tri->v0.y, tri->v0.z, tri->v1.x, tri->v1.y, tri->v1.z,
         tri->v2.x, tri->v2.y, tri->v2.z);
}

Hittable *triangle_create(Vec3 v0, Vec3 v1, Vec3 v2) {
  Triangle *triangle = malloc(sizeof(Triangle));
  assert(triangle != NULL);

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  triangle->v0 = v0;
  triangle->v1 = v1;
  triangle->v2 = v2;

  triangle->edge1 = vec3_sub(v1, v0); // plane a
  triangle->edge2 = vec3_sub(v2, v0); // plane b

  triangle->normal = vec3_normalized(
      vec3_cross(triangle->edge1, triangle->edge2)); // angle of normal line n

  hittable->type = HITTABLE_TRIANGLE;
  hittable->data = triangle;
  hittable->destroy = (HittableDestroyFn)triangle_destroy;
  hittable->hit = (HitFn)triangle_hit;

  return hittable;
}