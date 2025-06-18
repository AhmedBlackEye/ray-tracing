#ifndef AABB_H
#define AABB_H

#include <stdbool.h>

#include "interval.h"
#include "ray.h"
#include "vec3.h"

typedef struct AABB {
  Interval x;
  Interval y;
  Interval z;
} AABB;

static inline AABB aabb_empty() {
  return (AABB){interval_empty(), interval_empty(), interval_empty()};
}

static inline AABB aabb_make(Interval x, Interval y, Interval z) {
  return (AABB){x, y, z};
}

static inline AABB aabb_surrounding_box(AABB *box0, AABB *box1) {
  Interval x = interval_enclose(box0->x, box1->x);
  Interval y = interval_enclose(box0->y, box1->y);
  Interval z = interval_enclose(box0->z, box1->z);
  return (AABB){x, y, z};
}

static inline AABB aabb_from_points(Vec3 a, Vec3 b) {
  Interval x = (a.x <= b.x) ? (Interval){a.x, b.x} : (Interval){b.x, a.x};
  Interval y = (a.y <= b.y) ? (Interval){a.y, b.y} : (Interval){b.y, a.y};
  Interval z = (a.z <= b.z) ? (Interval){a.z, b.z} : (Interval){b.z, a.z};
  return (AABB){x, y, z};
}

static inline Interval axis_interval(AABB *box, int n) {
  if (n == 1)
    return box->y;
  if (n == 2)
    return box->z;
  return box->x;
}

// Returns the index of the longest axis of the bounding box.
static inline int aabb_longest_axis(AABB *self) {
  double x_size = interval_size(self->x);
  double y_size = interval_size(self->y);
  double z_size = interval_size(self->z);
  if (x_size > y_size) {
    return x_size > z_size ? 0 : 2;
  } else {
    return y_size > z_size ? 1 : 2;
  }
}

extern bool aabb_hit(AABB *box, Ray ray, Interval *ray_t);

#endif // AABB_H
