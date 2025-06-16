#ifndef CHECKERED_H
#define CHECKERED_H

#include "texture.h"

typedef struct Checkered {
    texture base_tex;  
    double inv_scale;
    texture* even;
    texture* odd;
} Checkered;

Color checkered_value(texture* self, double u, double v, const Vec3* p);
Checkered *checkered_create_textures(double scale, texture *even, texture *odd);
Checkered *checkered_create_colors(double scale, const Color *c1, const Color *c2);
void checkered_destroy(CHECKERED *checkered);

#endif // CHECKERED_H