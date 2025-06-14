#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"  

Hittable *quad_create(Vec3 Q, Vec3 u, Vec3 v);
void quad_print(const Hittable *hittable);

#endif // QUAD_H