#ifndef CHECKERED_H
#define CHECKERED_H

#include "texture.h"

typedef struct Checkered {
    Texture base_tex;  
    double inv_scale;
    Texture *even;
    Texture *odd;
} Checkered;

Color checkered_value(Texture *self, double u, double v, const Vec3* p);
Checkered *checkered_create_textures(double scale, Texture *even, Texture *odd);
Checkered *checkered_create_colors(double scale, const Color *c1, const Color *c2);
void checkered_destroy(Checkered *checkered);

#endif // CHECKERED_H