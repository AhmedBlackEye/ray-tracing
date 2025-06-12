#ifndef HITTABLE_H
#define HITTABLE_H

#include <stdbool.h>

#include "core/ray.h"
#include "core/vec3.h"
#include "core/interval.h"

typedef struct HitRecord {
  Vec3 p;
  Vec3 normal;
  double t;
  bool front_face;
} HitRecord;

static inline HitRecord hitrecord_zero(void) {
  return (HitRecord){
      .p = vec3_zero(), .normal = vec3_zero(), .t = 0.0, .front_face = false};
}

// Sets the hit record normal vector.
// NOTE: the parameter `outward_normal` is assumed to have unit length.
void hitrec_set_face_normal(HitRecord *rec, Ray r, Vec3 outward_normal);

typedef struct Hittable Hittable;
typedef bool (*HitFn)(const Hittable *self, Ray r, Interval t_bounds, HitRecord *rec);

typedef void (*HittableDestroyFn)(Hittable *self);
typedef void (*HittablePrintFn)(Hittable *self);

typedef enum { HITTABLE_SPHERE } HittableType;
typedef struct Hittable {
  HittableType type;
  HitFn hit;
  HittableDestroyFn destroy;
  void *data;
} Hittable;

// extern bool hittable_hit(const Hittable *self, Ray r, double ray_tmin,
//                          double ray_tmax, HitRecord *rec);

extern void hittable_destroy(Hittable *self);
extern void hittable_print(const Hittable *self);

#endif // HITTABLE_H