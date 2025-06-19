#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "material.h"
#include "core/color.h"
#include "core/vec3.h"
#include "texture/texture.h"

Material* diffuse_light_create(Color emit_color);
extern Color diffuse_light_emitted(Material *self, double u, double v, const Vec3 *p);
extern void diffuse_light_print(const Material *self);
extern Material *diffuse_light_create_texture(Texture *tex);

#endif // DIFFUSE_LIGHT_H