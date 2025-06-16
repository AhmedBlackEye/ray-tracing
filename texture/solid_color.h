#ifndef SOLID_COLOR_H
#define SOLID_COLOR_H

#include "texture.h"

typedef struct {
    texture base_tex;  
    color albedo;
} SolidColor;

Color solid_color_value(texture* self, double u, double v, const Vec3* p);
SOLIDCOLOR *solid_color_create_albedo(const Color *albedo);
SOLIDCOLOR *solid_color_create_rgb(double r, double g, double b);
void solid_color_destroy(SOLIDCOLOR* sol_col);

#endif // SOLID_COLOR_H