#ifndef PLANE_H
#define PLANE_H

#include "hittable.h"
#include "material/material.h"

extern Hittable *plane_create(Vec3 point, Vec3 normal, Material *mat);
extern void plane_print(const Hittable *hittable);

#endif // PLANE_H
