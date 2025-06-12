#include <math.h>

#define PI 3.1415926535897932385

#define ASPECT_RATIO (16.0 / 9.0)

#define WIDTH 400
#define HEIGHT ((int)(WIDTH / ASPECT_RATIO))

#define VIEWPORT_HEIGHT 2.0
#define VIEWPORT_WIDTH (VIEWPORT_HEIGHT * (double)WIDTH / HEIGHT)

#define FOCAL_LEN 1.0

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}
