#ifndef HITRECORD_H
#define HITRECORD_H

#include <stdbool.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hit_record.h"

typedef struct Material Material;

typedef struct HitRecord {
  Vec3 p;
  Vec3 normal;
  Material *mat;
  double t;
  bool front_face;
} HitRecord;

static inline HitRecord hitrecord_zero(void) {
  return (HitRecord){.p = vec3_zero(), .normal = vec3_zero(), .t = 0.0, .front_face = false};
}

// Sets the hit record normal vector.
// NOTE: the parameter `outward_normal` is assumed to have unit length.
void hitrec_set_face_normal(HitRecord *rec, Ray r, Vec3 outward_normal);

#endif // HITRECORD_H
