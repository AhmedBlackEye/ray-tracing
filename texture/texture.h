#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/color.h"

typedef struct Texture {
    Color (*value)(struct Texture *self, double u, double v, const Vec3 *p);
    void (*destroy)(struct Texture *self);
} Texture;

void texture_destroy(Texture *self);

#endif // TEXTURE_H