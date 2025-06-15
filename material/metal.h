#ifndef METAL_H
#define METAL_H

#include "core/color.h"
#include "material.h"

extern Material *metal_create(Color albedo, double fuzz);
extern void metal_print(const Material *self);

#endif // METAL_H