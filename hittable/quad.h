#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"  
#include "material/material.h"

Hittable *quad_create(Vec3 Q, Vec3 u, Vec3 v, Material *mat);
void quad_print(const Hittable *hittable);

#endif // QUAD_H