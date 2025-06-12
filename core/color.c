#include <stdio.h>

#include "color.h"
#include "interval.h"

void write_color(FILE *out, Color pixel_color) {
  Interval intensity = interval_make(0.000, 0.999);
  int r = 256 * interval_clamp(intensity, pixel_color.x);
  int g = 256 * interval_clamp(intensity, pixel_color.y);
  int b = 256 * interval_clamp(intensity, pixel_color.z);

  fprintf(out, "%d %d %d\n", r, g, b);
}
