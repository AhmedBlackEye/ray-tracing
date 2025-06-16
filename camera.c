#include <math.h>

#include "camera.h"
#include "core/color.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "material/material.h"
#include "shared.h"

// Create a new camera instance
Camera camera_make(int image_width, double aspect_ratio, Vec3 lookfrom,
                   Vec3 lookat, Vec3 vup, double vfov, double defocus_angle,
                   double focus_dist, int samples_per_pixel, int max_depth,
                   Color background) {
  Camera cam = {0}; // Initialize all fields to zero

  // Basic camera parameters
  cam.image_width = image_width;
  cam.aspect_ratio = aspect_ratio;
  cam.samples_per_pixel = samples_per_pixel;
  cam.max_depth = max_depth;
  cam.vfov = vfov;
  cam.defocus_angle = defocus_angle;
  cam.focus_dist = focus_dist;
  cam.lookfrom = lookfrom;
  cam.lookat = lookat;
  cam.vup = vup;
  cam.background = background;
  cam.center = lookfrom;

  // Calculate image height with proper bounds checking
  cam.image_height = (int)(image_width / aspect_ratio);
  if (cam.image_height < 1)
    cam.image_height = 1;

  cam.pixel_samples_scale = 1.0 / cam.samples_per_pixel;

  // Calculate viewport dimensions
  double theta = degrees_to_radians(cam.vfov);
  double h = tan(theta / 2.0);
  double viewport_height = 2.0 * h * cam.focus_dist;
  double viewport_width =
      viewport_height * (double)cam.image_width / cam.image_height;

  // Calculate orthonormal basis vectors for camera coordinate system
  cam.w = vec3_normalized(
      vec3_sub(cam.lookfrom, cam.lookat));             // Points toward viewer
  cam.u = vec3_normalized(vec3_cross(cam.vup, cam.w)); // Points right
  cam.v = vec3_cross(cam.w, cam.u);                    // Points up

  // Calculate viewport vectors and pixel deltas
  Vec3 viewport_u = vec3_scale(cam.u, viewport_width);
  Vec3 viewport_v = vec3_scale(vec3_negate(cam.v), viewport_height);
  cam.pixel_delta_u = vec3_divs(viewport_u, cam.image_width);
  cam.pixel_delta_v = vec3_divs(viewport_v, cam.image_height);

  // Calculate viewport upper-left corner and first pixel location
  Vec3 viewport_upper_left =
      vec3_sub(cam.center, vec3_scale(cam.w, cam.focus_dist));
  viewport_upper_left =
      vec3_sub(viewport_upper_left, vec3_divs(viewport_u, 2.0));
  viewport_upper_left =
      vec3_sub(viewport_upper_left, vec3_divs(viewport_v, 2.0));

  cam.pixel00_loc =
      vec3_add(viewport_upper_left,
               vec3_scale(vec3_add(cam.pixel_delta_u, cam.pixel_delta_v), 0.5));

  // Calculate camera defocus
  double defocus_radius =
      cam.focus_dist * tan(degrees_to_radians(defocus_angle / 2));
  cam.defocus_disk_u = vec3_scale(cam.u, defocus_radius);
  cam.defocus_disk_v = vec3_scale(cam.v, defocus_radius);

  return cam;
}

static Color ray_color(Ray r, int depth, DynArray *hittable_world, Color background) {
  if (depth <= 0)
    return vec3_zero();
  HitRecord rec;

  if (!hittables_hit(hittable_world, r, interval_make(1e-4, INFINITY), &rec)) {
        return background;
  }

  Ray scatterd;
  Color attenuation;
  Color color_from_emission = material_emitted(rec.mat, 0.0, 0.0, &rec.p);

  if (!rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scatterd)) {
    return color_from_emission;
  }
  Color color_from_scatter = vec3_mul(ray_color(scatterd, depth - 1, hittable_world, background),
                                      attenuation);
  return vec3_add(color_from_emission, color_from_scatter);
}

// Construct a camera ray originating from the defocus disk and directed at a
// randomly sampled point around the pixel location i, j.
static Ray get_ray(const Camera *cam, int i, int j) {
  Vec3 offset = vec3_sample_square();
  Vec3 pixel_sample =
      vec3_add(cam->pixel00_loc, vec3_scale(cam->pixel_delta_u, i + offset.x));
  pixel_sample =
      vec3_add(pixel_sample, vec3_scale(cam->pixel_delta_v, j + offset.y));

  Vec3 ray_origin;
  if (cam->defocus_angle <= 0) {
    ray_origin = cam->center;
  } else {
    ray_origin = defocus_disk_sample(cam->center, cam->defocus_disk_u,
                                     cam->defocus_disk_v);
  }

  return (Ray){.origin = ray_origin,
               .direction = vec3_sub(pixel_sample, ray_origin)};
}

void camera_render(const Camera *cam, DynArray *hittable_world,
                   FILE *out_file) {
  fprintf(out_file, "P3\n%d %d\n255\n", cam->image_width, cam->image_height);
  for (int j = 0; j < cam->image_height; j++) {
    update_progress_bar(j + 1, cam->image_height);
    for (int i = 0; i < cam->image_width; i++) {
      Vec3 pixel_color = vec3_zero();
      for (int sample = 0; sample < cam->samples_per_pixel; sample++) {
        Ray r = get_ray(cam, i, j);
        pixel_color =
            vec3_add(pixel_color, ray_color(r, cam->max_depth, hittable_world, cam->background));
      }
      write_color(out_file, vec3_divs(pixel_color, cam->samples_per_pixel));
    }
  }
}