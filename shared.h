#ifndef SHARED_H
#define SHARED_H

#include <math.h>
#include <stdlib.h>

#include "core/vec3.h"

#define PI 3.1415926535897932385

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static inline double degrees_to_radians(double degrees) {
  return degrees * PI / 180.0;
}

// Returns a random real in [0, 1)
static inline double random_double() { 
    return rand() / (RAND_MAX + 1.0); 
}

// Returns a random real in [min, max)
static inline double random_double_range(double min, double max) {
  return min + (max - min) * random_double();
}

// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
// square.
static inline Vec3 sample_square() {
  return (Vec3){random_double() - 0.5, random_double() - 0.5, 0};
}

#endif // SHARED_H