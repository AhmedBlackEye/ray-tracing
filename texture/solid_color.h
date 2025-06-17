#ifndef SOLID_COLOR_H
#define SOLID_COLOR_H

#include "texture.h"

typedef struct SolidColor{
    Texture base_tex;  
    Color albedo;
} SolidColor;

Color solid_color_value(Texture *self, double u, double v, const Vec3* p);
SolidColor *solid_color_create_albedo(const Color *albedo);
SolidColor *solid_color_create_rgb(double r, double g, double b);
void solid_color_destroy(SolidColor* sol_col);

#endif // SOLID_COLOR_H