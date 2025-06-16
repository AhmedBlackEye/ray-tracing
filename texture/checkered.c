#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "checkered.h"
#include "solid_color.h"

Color checkered_value(TEXTURE* self, double u, double v, const Vec3* p) {
    Checkered* checkered = (Checkered*)self;

    int x_int = floor(checkered->inv_scale * p->x);
    int y_int = floor(checkered->inv_scale * p->y);
    int z_int = floor(checkered->inv_scale * p->z);

    bool is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? checkered->even->value(checkered->even, u, v, p) 
                   : checkered->odd->value(checkered->odd, u, v, p);
}

Checkered *checkered_create_textures(double scale, TEXTURE *even, TEXTURE *odd) {
    Checkered *checkered = malloc(sizeof(struct Checkered));
    assert(checkered != NULL);
    checkered->base_tex.value = checkered_value;
    checkered->inv_scale = 1.0 / scale;
    checkered->even = even;
    checkered->odd = odd;
    return checkered;
}

Checkered *checkered_create_colors(double scale, const Color *c1, const Color *c2) {
    SOLIDCOLOR *even = solid_color_create_albedo(c1);
    SOLIDCOLOR *odd = solid_color_create_albedo(c2);
    return checkered_create_textures(scale, (TEXTURE*)even, (TEXTURE*)odd);
}

void checkered_destroy(Checkered* checkered) {
    assert(checkered != NULL);
    free(checkered);
}