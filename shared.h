#ifndef SHARED_H
#define SHARED_H

#include <math.h>
#include <stdlib.h>

#define PI 3.1415926535897932385

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static inline double degrees_to_radians(double degrees) {
  return degrees * PI / 180.0;
}

// Returns a random real in [0, 1)
static inline double random_double(void) { return rand() / (RAND_MAX + 1.0); }

// Returns a random real in [min, max)
static inline double random_double_range(double min, double max) {
  return min + (max - min) * random_double();
}

extern void update_progress_bar(int current, int total);
extern void update_simple_progress_bar(int current, int total);

#endif // SHARED_H