#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "material.h"
#include "core/color.h"
#include "core/vec3.h"

extern Material* diffuse_light_create_from_texture(Texture *tex);
extern Material* diffuse_light_create_from_color(Color emit_color);

extern Color diffuse_light_emitted(Material *self, double u, double v, const Vec3 *p);

extern void diffuse_light_print(const Material *self);

#endif // DIFFUSE_LIGHT_H