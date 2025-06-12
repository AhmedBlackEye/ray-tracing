#include <stdio.h>

#include "color.h"

void write_color(FILE *out, Color pixel_color) {
  int r = 255.999 * pixel_color.x;
  int g = 255.999 * pixel_color.y;
  int b = 255.999 * pixel_color.z;

  fprintf(out, "%d %d %d\n", r, g, b);
}
