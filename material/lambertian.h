#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "core/color.h"
#include "material.h"

extern Material *lambertian_create(Color albedo);
extern void lambertian_print(const Material *self);

#endif // LAMBERTIAN_H