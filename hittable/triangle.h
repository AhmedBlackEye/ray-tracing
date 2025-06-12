#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../core/vec3.h"
#include "../core/ray.h"
#include "../core/interval.h"

#define EPSILON 1e-8
extern Triangle *triangle_create(Vec3 v0, Vec3 v1, Vec3 v2);

#endif // TRIANGLE_H