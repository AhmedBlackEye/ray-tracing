#include <math.h>

#define PI 3.1415926535897932385

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}
