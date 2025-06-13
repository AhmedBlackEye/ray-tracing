#ifndef CAMERA_H
#define CAMERA_H

#include "core/color.h"
#include "core/dyn_array.h"
#include "core/vec3.h"
#include "hittable/hittable.h"

typedef struct Camera {
  int image_width;
  int image_height;
  int samples_per_pixel;
  double aspect_ratio;

  double pixel_samples_scale;
  Vec3 center;
  Vec3 pixel00_loc;
  Vec3 pixel_delta_u;
  Vec3 pixel_delta_v;
} Camera;

// Create a new camera instance
extern Camera camera_make(int image_width, double aspect_ratio);
extern void camera_render(const Camera *cam, DynArray *hittable_world,
                          FILE *out_file);

#endif // CAMERA_H