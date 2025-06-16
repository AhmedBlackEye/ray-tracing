#include <assert.h>
#include <stdlib.h> 

#include "solid_color.h"
#include "core/color.h"

color solid_color_value(texture* self, double u, double v, const Vec3* p) {
    solid_color* sol_col = (solid_color*)self;
    return sol_col->albedo;
}

solid_color* solid_color_create_albedo(const Color* albedo) {
    solid_color* sol_col = malloc(sizeof(struct SOLIDCOLOR));
    assert(sol_col != NULL);
    sol_col->base_tex.value = solid_color_value;
    sol_col->albedo = *albedo;
    return sol_col;
}

solid_color* solid_color_create_rgb(double r, double g, double b) {
    Color albedo = {r, g, b};
    return solid_color_create_albedo(&albedo);
}

void solid_color_destroy(solid_color* sol_col) {
    assert(sol_col != NULL);
    free(sol_col);
}