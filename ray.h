#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
  Vec3 offset;
  Vec3 direction;
} Ray;

static inline Vec3 ray_at(Ray r, double pos) {
  return vec3_add(vec3_scale(r.direction, pos), r.offset);
}

#endif // RAY_H