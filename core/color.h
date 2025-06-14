#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>

#include "vec3.h"

typedef Vec3 Color;

void write_color(FILE *out, Color pixel_color);
void color_print(Color c);

#endif // COLOR_H
