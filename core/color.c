#include <math.h>
#include <stdio.h>

#include "color.h"
#include "interval.h"

static inline double linear_to_gamma(double linear_component) {
  return linear_component > 0 ? sqrt(linear_component) : 0;
}

void write_color(FILE *out, Color pixel_color) {
  // Apply a linear to gamma transform for gamma 2
  double r = linear_to_gamma(pixel_color.x);
  double g = linear_to_gamma(pixel_color.y);
  double b = linear_to_gamma(pixel_color.z);

  Interval intensity = interval_make(0.000, 0.999);
  int rbyte = 256 * interval_clamp(intensity, r);
  int gbyte = 256 * interval_clamp(intensity, g);
  int bbyte = 256 * interval_clamp(intensity, b);

  fprintf(out, "%d %d %d\n", rbyte, gbyte, bbyte);
}

void color_print(Color c) {
  printf("Color { r: %f, %f, %f}", c.x, c.y, c.z);
}
