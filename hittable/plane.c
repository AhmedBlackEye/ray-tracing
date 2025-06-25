#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hit_record.h"
#include "hittable.h"
#include "material/material.h"
#include "plane.h"

typedef struct Plane {
  Vec3 point;
  Vec3 normal;
} Plane;

static bool plane_hit(const Hittable *self, Ray ray, Interval t_bounds,
                      HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  const Plane *plane = (const Plane *)self->data;

  double denom = vec3_dot(ray.direction, plane->normal);
  if (fabs(denom) < DBL_EPSILON) {
    return false;
  }

  Vec3 point_minus_origin = vec3_sub(plane->point, ray.origin);
  double t = vec3_dot(point_minus_origin, plane->normal) / denom;

  if (!interval_surrounds(t_bounds, t)) {
    return false;
  }

  rec->mat = self->mat;
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

Hittable *plane_create(Vec3 point, Vec3 normal, Material *mat) {
  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  Plane *plane_data = malloc(sizeof(struct Plane));
  assert(plane_data != NULL);

  plane_data->point = point;
  plane_data->normal = vec3_normalized(normal);

  hittable->type = HITTABLE_PLANE;
  hittable->hit = plane_hit;
  hittable->destroy = (HittableDestroyFn)plane_destroy;
  hittable->mat = mat;
  hittable->data = plane_data;

  const double PLANE_EXTENT = 1000.0; // Large but finite extent

  // Determine which axis the plane is most aligned with
  Vec3 abs_normal = {fabs(normal.x), fabs(normal.y), fabs(normal.z)};

  if (abs_normal.y > abs_normal.x && abs_normal.y > abs_normal.z) {
    // Horizontal plane (mostly Y-aligned normal)
    double y = point.y;
    hittable->bbox =
        aabb_make(interval_make(-PLANE_EXTENT, PLANE_EXTENT), // x
                  interval_make(y - 0.001, y + 0.001),        // y (thin slice)
                  interval_make(-PLANE_EXTENT, PLANE_EXTENT)  // z
        );
  } else if (abs_normal.x > abs_normal.z) {
    // Vertical plane (mostly X-aligned normal)
    double x = point.x;
    hittable->bbox =
        aabb_make(interval_make(x - 0.001, x + 0.001),        // x (thin slice)
                  interval_make(-PLANE_EXTENT, PLANE_EXTENT), // y
                  interval_make(-PLANE_EXTENT, PLANE_EXTENT)  // z
        );
  } else {
    // Vertical plane (mostly Z-aligned normal)
    double z = point.z;
    hittable->bbox =
        aabb_make(interval_make(-PLANE_EXTENT, PLANE_EXTENT), // x
                  interval_make(-PLANE_EXTENT, PLANE_EXTENT), // y
                  interval_make(z - 0.001, z + 0.001)         // z (thin slice)
        );
  }

  return hittable;
}

void plane_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_PLANE) {
    printf("Plane: Invalid or NULL\n");
    return;
  }
  const Plane *plane = (const Plane *)hittable->data;
  printf("Plane { point: (%.3f, %.3f, %.3f), normal: (%.3f, %.3f, %.3f) }\n",
         plane->point.x, plane->point.y, plane->point.z, plane->normal.x,
         plane->normal.y, plane->normal.z);
}
