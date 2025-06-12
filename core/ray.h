#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct Ray {
  Vec3 origin;
  Vec3 direction;
} Ray;

static inline Vec3 ray_at(Ray r, double pos) {
  return vec3_add(vec3_scale(r.direction, pos), r.origin);
}

#endif // RAY_H
