#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "core/color.h"
#include "material.h"

extern Material *dielectric_create(double refraction_index);
extern void dielectric_print(const Material *self);

#endif // DIELECTRIC_H