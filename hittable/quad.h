#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"  
#include "material/material.h"
#include "core/dyn_array.h"

Hittable *quad_create(Vec3 Q, Vec3 u, Vec3 v, Material *mat);
void quad_print(const Hittable *hittable);
DynArray *box_create(const Vec3 a, const Vec3 b, Material* mat);

#endif // QUAD_H