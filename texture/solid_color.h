#ifndef SOLID_COLOR_H
#define SOLID_COLOR_H

#include "texture.h"

typedef struct {
    texture base_tex;  
    color albedo;
} SOLIDCOLOR;

Color solid_color_value(texture* self, double u, double v, const Vec3* p);
solid_color *solid_color_create_albedo(const color *albedo);
solid_color *solid_color_create_rgb(double r, double g, double b);
void solid_color_destroy(SOLIDCOLOR* sol_col);

#endif // SOLID_COLOR_H