#ifndef HITTABLE_H
#define HITTABLE_H

#include <stdbool.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "material/material.h"

typedef struct Hittable Hittable;
typedef bool (*HitFn)(const Hittable *self, Ray r, Interval t_bounds,
                      HitRecord *rec);

typedef void (*HittableDestroyFn)(Hittable *self);
typedef void (*HittablePrintFn)(Hittable *self);

typedef enum {
  HITTABLE_SPHERE,
  HITTABLE_PLANE,
  HITTABLE_TRIANGLE,
  HITTABLE_QUAD
} HittableType;

typedef struct Hittable {
  HittableType type;
  HitFn hit;
  HittableDestroyFn destroy;
  Material *mat;
  void *data;
} Hittable;

extern void hittable_destroy(Hittable *self);
extern void hittable_print(const Hittable *self);

#endif // HITTABLE_H
