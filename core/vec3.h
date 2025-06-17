#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "shared.h"

#define DBL_EPSILON 1e-6

typedef struct Vec3 {
  double x, y, z;
} Vec3;

static inline Vec3 vec3_add(Vec3 v1, Vec3 v2) {
  return (Vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

static inline Vec3 vec3_adds(Vec3 v1, double d) {
  return (Vec3){v1.x + d, v1.y + d, v1.z + d};
}

static inline Vec3 vec3_sub(Vec3 v1, Vec3 v2) {
  return (Vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

static inline Vec3 vec3_subs(Vec3 v1, double d) {
  return (Vec3){v1.x - d, v1.y - d, v1.z - d};
}

static inline Vec3 vec3_mul(Vec3 v1, Vec3 v2) {
  return (Vec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

static inline Vec3 vec3_scale(Vec3 v1, double d) {
  return (Vec3){v1.x * d, v1.y * d, v1.z * d};
}

static inline Vec3 vec3_div(Vec3 v1, Vec3 v2) {
  return (Vec3){v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
}

static inline Vec3 vec3_divs(Vec3 v1, double d) {
  return (Vec3){v1.x / d, v1.y / d, v1.z / d};
}

static inline double vec3_dot(Vec3 v1, Vec3 v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline Vec3 vec3_negate(Vec3 v) { return vec3_scale(v, -1.0); }

static inline Vec3 vec3_cross(Vec3 v1, Vec3 v2) {
  return (Vec3){v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
                v1.x * v2.y - v1.y * v2.x};
}

static inline double vec3_length(Vec3 v) {
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline double vec3_length_squared(Vec3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline Vec3 vec3_zero(void) { return (Vec3){0.0, 0.0, 0.0}; }

static inline Vec3 vec3_one(void) { return (Vec3){1.0, 1.0, 1.0}; }

static inline Vec3 vec3_normalized(Vec3 v) {
  double len = vec3_length(v);
  return len > 0.0 ? vec3_divs(v, len) : vec3_zero();
}

static inline void vec3_normalize(Vec3 *v) {
  double len = vec3_length(*v);
  *v = len > 0.0 ? vec3_divs(*v, len) : vec3_zero();
}

static inline int vec3_equal(Vec3 v1, Vec3 v2) {
  return fabs(v1.x - v2.x) < DBL_EPSILON && fabs(v1.y - v2.y) < DBL_EPSILON &&
         fabs(v1.z - v2.z) < DBL_EPSILON;
}

// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
// square.
static inline Vec3 vec3_sample_square(void) {
  return (Vec3){random_double() - 0.5, random_double() - 0.5, 0};
}

static inline Vec3 vec3_random(void) {
  return (Vec3){random_double(), random_double(), random_double()};
}

static inline Vec3 vec3_random_bounded(double min, double max) {
  return (Vec3){random_double_range(min, max), random_double_range(min, max),
                random_double_range(min, max)};
}

static inline Vec3 vec3_random_unit_vector(void) {
  while (true) {
    Vec3 p = vec3_random_bounded(-1, 1);
    double lensq = vec3_length_squared(p);
    if (1e-160 < lensq && lensq <= 1)
      return vec3_divs(p, sqrt(lensq));
  }
}

static inline Vec3 vec3_random_on_hemisphere(Vec3 normal) {
  Vec3 on_unit_sphere = vec3_random_unit_vector();
  bool on_same_hemisphere = vec3_dot(on_unit_sphere, normal) > 0.0;
  return on_same_hemisphere ? on_unit_sphere : vec3_scale(on_unit_sphere, -1);
}

static inline Vec3 vec3_random_in_unit_disk(void) {
  while (true) {
    Vec3 p = {random_double_range(-1, 1), random_double_range(-1, 1), 0};
    if (vec3_length_squared(p) < 1)
      return p;
  }
}

static inline Vec3 defocus_disk_sample(Vec3 center, Vec3 defocus_disk_u,
                                       Vec3 defocus_disk_v) {
  Vec3 p = vec3_random_in_unit_disk();
  Vec3 res = vec3_add(center, vec3_scale(defocus_disk_u, p.x));
  return vec3_add(res, vec3_scale(defocus_disk_v, p.y));
}

static inline void vec3_print(Vec3 v) {
  printf("Vec3<%f, %f, %f>", v.x, v.y, v.z);
}

// Return true if the vector is close to zero in all dimensions.
static inline bool vec3_is_near_zero(Vec3 v) {
  double s = 1e-8;
  return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

static inline Vec3 vec3_reflect(Vec3 v, Vec3 n) {
  return vec3_sub(v, vec3_scale(n, 2 * vec3_dot(v, n)));
}

static inline Vec3 vec3_refract(Vec3 uv, Vec3 n, double etai_over_etat) {
  double cos_theta = fmin(vec3_dot(vec3_negate(uv), n), 1.0);
  Vec3 r_out_perp =
      vec3_scale(vec3_add(uv, vec3_scale(n, cos_theta)), etai_over_etat);
  Vec3 r_out_parallel =
      vec3_scale(n, -sqrt(fabs(1.0 - vec3_length_squared(r_out_perp))));
  return vec3_add(r_out_perp, r_out_parallel);
}

static inline double vec3_axis(Vec3 v, int axis) {
  if (axis == 0) {
    return v.x;
  } else if (axis == 1) {
    return v.y;
  }
  return v.z;
}
#endif // VEC3_H
