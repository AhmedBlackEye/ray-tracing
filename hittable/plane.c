#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "hittable.h"

typedef struct Plane {
  Vec3 point;
  Vec3 normal;
} Plane;

static bool plane_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  const Plane *plane = (const Plane *)self->data;

  double denominator = vec3_dot(ray.direction, plane->normal);
  if (fabs(denominator) < DBL_EPSILON) {
    return false;
  }

  Vec3 point_minus_origin = vec3_sub(plane->point, ray.origin);  
  double t = vec3_dot(point_minus_origin, plane->normal) / denominator;

  if (!interval_surrounds(t_bounds, t)) {
        return false;
  }

  rec->t = t;
  rec->p = ray_at(ray, rec->t);
  hitrec_set_face_normal(rec, ray, plane->normal);

  return true;
}

static void plane_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

Hittable *plane_create(Vec3 point, Vec3 normal) {
  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  Plane *plane_data = malloc(sizeof(struct Plane));
  assert(plane_data != NULL);

  plane_data->point = point;
  plane_data->normal = vec3_normalized(normal);

  hittable->type = HITTABLE_PLANE;
  hittable->hit = plane_hit;
  hittable->destroy = (HittableDestroyFn)plane_destroy;
  hittable->data = plane_data;

  return hittable;
}

void plane_print(const Plane *plane) {
  if (plane == NULL) {
    printf("Plane: NULL\n");
    return;
  }

  printf("Plane { point: (%.3f, %.3f, %.3f), normal: (%.3f, %.3f, %.3f) }\n",
         plane->point.x, plane->point.y, plane->point.z,
         plane->normal.x, plane->normal.y, plane->normal.z);
}