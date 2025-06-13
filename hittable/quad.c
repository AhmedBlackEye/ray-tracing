#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "quad.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct Quad {
    Vec3 Q;   
    Vec3 u
    Vec3 v;
    Vec3 normal;
    Vec3 w;  
    double D;
} Quad;

static bool sphere_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  const Sphere *sphere = (const Sphere *)self->data;

  Vec3 oc = vec3_sub(sphere->center, ray.origin);
  double a = vec3_length_squared(ray.direction);
  double h = vec3_dot(ray.direction, oc);
  double c = vec3_length_squared(oc) - sphere->radius * sphere->radius;

  double discriminant = h * h - a * c;

  if (discriminant < 0) {
    return false;
  }
  double disc_sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range
  double root = (h - disc_sqrtd) / a;
  if (!interval_surrounds(t_bounds, root)) {
    root = (h + disc_sqrtd) / a;
    if (!interval_surrounds(t_bounds, root))
      return false;
  }

  rec->t = root;
  rec->p = ray_at(ray, rec->t);
  Vec3 outward_normal =
      vec3_divs(vec3_sub(rec->p, sphere->center), sphere->radius);
  hitrec_set_face_normal(rec, ray, outward_normal);

  return true;
}

static void quad_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

Hittable *quad_create(Vec3 Q, Vec3 u, Vec3 v) {

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  Quad *squad_data = malloc(sizeof(struct Quad));
  assert(quad_data != NULL);

  quad_data->Q = Q;
  quad_data->u = u;
  quad_data->v = v;

  Vec3 n = vec3_cross(u, v);
  quad_data->normal = vec3_unit(n);

  hittable->type = QUAD_SPHERE;
  hittable->hit = quad_hit;
  hittable->destroy = (HittableDestroyFn)quad_destroy;
  hittable->data = quad_data;

  return hittable;
}

void quad_print(const Hittable *hittable) {
    if (hittable == NULL || hittable->type != HITTABLE_QUAD) {
        printf("Quad: Invalid or NULL\n");
        return;
    }
    const Quad *quad = (const Quad *)hittable->data;
    printf(
        "Quad { Q: (%.3f, %.3f, %.3f), u: (%.3f, %.3f, %.3f), v: (%.3f, %.3f, %.3f) }\n",
        quad->Q.x, quad->Q.y, quad->Q.z,
        quad->u.x, quad->u.y, quad->u.z,
        quad->v.x, quad->v.y, quad->v.z
    );
}