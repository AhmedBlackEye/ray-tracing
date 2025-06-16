#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "core/color.h"
#include "material.h"
#include "texture/texture.h"

extern Material *lambertian_create(Color albedo);
extern void lambertian_print(const Material *self);
extern Material *lambertian_create_texture(Texture *tex);

#endif // LAMBERTIAN_H