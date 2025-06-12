#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

extern Hittable *sphere_create(Vec3 center, double radius);
extern void sphere_print(const Sphere *sphere);

#endif // SPHERE_H
