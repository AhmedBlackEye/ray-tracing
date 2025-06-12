#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "hittable.h"

typedef struct Sphere {
  Vec3 center;
  double radius;
} Sphere;

static bool sphere_hit(const Hittable *self, Ray ray, double ray_tmin,
                       double ray_tmax, HitRecord *rec) {
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
  if (root <= ray_tmin || ray_tmax <= root) {
    root = (h + disc_sqrtd) / a;
    if (root <= ray_tmin || ray_tmax <= root)
      return false;
  }

  rec->t = root;
  rec->p = ray_at(ray, rec->t);
  rec->normal = vec3_divs(vec3_sub(rec->p, sphere->center), sphere->radius);

  return true;
}

static void sphere_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

Hittable *sphere_create(Vec3 center, double radius) {
  assert(radius > 0);

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  Sphere *sphere_data = malloc(sizeof(struct Sphere));
  assert(sphere_data != NULL);

  sphere_data->center = center;
  sphere_data->radius = radius;

  hittable->type = HITTABLE_SPHERE;
  hittable->hit = sphere_hit;
  hittable->destroy = (DestroyFn)sphere_destroy;
  hittable->data = sphere_data;

  return hittable;
}