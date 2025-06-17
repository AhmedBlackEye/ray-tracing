#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material/material.h"

extern Hittable *sphere_create(Vec3 center, double radius, Material *mat);
extern void sphere_print(const Hittable *hittable);
extern Hittable *sphere_create_moving(Vec3 center_start, Vec3 center_end, double radius, Material *mat);

#endif // SPHERE_H
