#ifndef CAMERA_H
#define CAMERA_H

#include "core/color.h"
#include "core/dyn_array.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include <stdbool.h>

typedef struct Camera {
  int image_width;
  int image_height;
  int samples_per_pixel;
  int max_depth;
  double aspect_ratio;
  // Vertical view angle (field of view)
  double vfov;
  double defocus_angle;
  double focus_dist;
  Vec3 lookfrom;
  Vec3 lookat;
  Vec3 vup;
  Color background;

  // computed
  double pixel_samples_scale;
  Vec3 center;
  Vec3 pixel00_loc;
  Vec3 pixel_delta_u;
  Vec3 pixel_delta_v;
  Vec3 u, v, w;
  Vec3 defocus_disk_u;
  Vec3 defocus_disk_v;
} Camera;

// Create a new camera instance
extern Camera camera_make(int image_width, double aspect_ratio, Vec3 lookfrom,
                          Vec3 lookat, Vec3 vup, double vfov,
                          double defocus_angle, double focus_dist,
                          int samples_per_pixel, int max_depth,
                          Color background, bool is_lighting);
extern void camera_render(const Camera *cam, Hittable *hittable_world,
                          FILE *out_file);

#endif // CAMERA_H