#ifndef METAL_H
#define METAL_H

#include "core/color.h"
#include "material.h"
#include "texture/texture.h"

extern Material *metal_create(Color albedo, double fuzz);
extern void metal_print(const Material *self);
extern Material *metal_create_texture(Texture* tex, double fuzz);            // Add this new one

#endif // METAL_H