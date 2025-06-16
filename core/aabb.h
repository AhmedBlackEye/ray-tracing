#ifndef AABB_H
#define AABB_H

#include <stdbool.h>

#include "interval.h"
#include "vec3.h"
#include "ray.h"

typedef struct {
    Interval x;
    Interval y;
    Interval z;
} AABB;

static inline AABB aabb_empty() {
    return (AABB){ interval_empty(), interval_empty(), interval_empty() };
}

static inline AABB aabb_make(Interval x, Interval y, Interval z) {
    return (AABB){ x, y, z };
}

static inline AABB aabb_from_points(Vec3 a, Vec3 b) {
    Interval x = (a.x <= b.x) ? (Interval) { a.x, b.x } : (Interval) { b.x, a.x };
    Interval y = (a.y <= b.y) ? (Interval) { a.y, b.y } : (Interval) { b.y, a.y };
    Interval z = (a.z <= b.z) ? (Interval) { a.z, b.z } : (Interval) { b.z, a.z };
    return (AABB){ x, y, z };
}

static inline Interval* axis_interval(AABB *box, int n) {
    if (n == 1) return &box->y;
    if (n == 2) return &box->z;
    return &box->x;
}

bool aabb_hit(AABB *box, Ray *r, Interval *ray_t);
