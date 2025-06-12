#ifndef VEC3_H
#define VEC3_H

#include <math.h>

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

#endif // VEC3_H
