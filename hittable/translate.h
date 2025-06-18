#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "core/vec3.h"
#include "hittable.h"

Hittable *translate_create(Hittable* object, Vec3 offset);
void translate_print(const Hittable *hittable);

#endif // TRANSLATE_H