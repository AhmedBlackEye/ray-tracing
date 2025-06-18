#ifndef ROTATE_Y_H
#define ROTATE_Y_H

#include "core/vec3.h"
#include "hittable.h"

Hittable *rotate_y_create(Hittable* object, double angle_degrees);
void rotate_y_print(const Hittable *hittable);

#endif // ROTATE_Y_H
