#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
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
    // printf("[DEBUG] Triangle hit at t=%.2f, p=(%.2f, %.2f, %.2f)\n", t,
    //        rec->p.x, rec->p.y, rec->p.z);

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

void triangle_raw_print(const TriangleRaw *tri) {
  if (tri == NULL) {
    printf("Triangle: NULL.\n");
    return;
  }
  printf("Triangle v0:(%.2f,%.2f,%.2f), v1:(%.2f, %.2f, %.2f), "
         "v2:(%.2f,%.2f,%.2f)\n",
         tri->v0.x, tri->v0.y, tri->v0.z, tri->v1.x, tri->v1.y, tri->v1.z,
         tri->v2.x, tri->v2.y, tri->v2.z);
}

TriangleRaw *triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2) {
  TriangleRaw *triangle = malloc(sizeof(Triangle));
  assert(triangle != NULL);

  triangle->v0 = v0;
  triangle->v1 = v1;
  triangle->v2 = v2;

  triangle->edge1 = vec3_sub(v1, v0); // plane a
  triangle->edge2 = vec3_sub(v2, v0); // plane b

  // printf("x: %f , y: %f, z: %f \n", triangle->edge1.x, triangle->edge1.y,
  //  triangle->edge1.z);

  triangle->normal = vec3_normalized(
      vec3_cross(triangle->edge1, triangle->edge2)); // angle of normal line n

  return triangle;
}