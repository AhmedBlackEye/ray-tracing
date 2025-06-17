#include <assert.h>
#include <stdlib.h> 

#include "solid_color.h"
#include "core/color.h"

Color solid_color_value(Texture *self, double u, double v, const Vec3* p) {
    SolidColor* sol_col = (SolidColor*)self;
    return sol_col->albedo;
}

SolidColor *solid_color_create_albedo(const Color* albedo) {
    SolidColor* sol_col = malloc(sizeof(struct SolidColor));
    assert(sol_col != NULL);
    sol_col->base_tex.value = solid_color_value;
    sol_col->albedo = *albedo;
    return sol_col;
}

SolidColor *solid_color_create_rgb(double r, double g, double b) {
    Color albedo = {r, g, b};
    return solid_color_create_albedo(&albedo);
}

void solid_color_destroy(SolidColor *sol_col) {
    assert(sol_col != NULL);
    free(sol_col);
}