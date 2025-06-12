#ifndef HITTABLE_H
#define HITTABLE_H

#include <stdbool.h>

#include "core/vec3.h"
#include "core/ray.h"

typedef struct HitRecord {
  Vec3 p;
  Vec3 normal;
  double t;
} HitRecord;

static inline HitRecord hitrecord_zero(void) {
  return (HitRecord){.p = vec3_zero(), .normal = vec3_zero(), .t = 0.0};
}

typedef struct Hittable Hittable;
typedef bool (*HitFn)(const Hittable *self, Ray r, double ray_tmin,
                      double ray_tmax, HitRecord *rec);
typedef void (*DestroyFn)(Hittable *self);

typedef enum { HITTABLE_SPHERE } HittableType;
typedef struct Hittable {
  HittableType type;
  HitFn hit;
  DestroyFn destroy;
  void *data;
} Hittable;

#endif // HITTABLE_H