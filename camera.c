
#include "camera.h"
#include "core/color.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "shared.h"

#define VIEWPORT_HEIGHT 2.0
#define FOCAL_LENGTH  1.0
#define SAMPLES_PER_PIXEL 10

// Create a new camera instance
Camera camera_make(int image_width, double aspect_ratio) {
  Camera camera;
  camera.image_width = image_width;
  camera.aspect_ratio = aspect_ratio;
  camera.image_height = (int)(image_width / aspect_ratio);

  double viewport_height = VIEWPORT_HEIGHT;
  double viewport_width =
      viewport_height * (double)camera.image_width / camera.image_height;
  double focal_length = FOCAL_LENGTH;

  camera.center = vec3_zero();

  Vec3 half_viewport_u = (Vec3){viewport_width / 2, 0, 0};
  Vec3 half_viewport_v = (Vec3){0, -viewport_height / 2, 0};

  camera.pixel_delta_u = (Vec3){viewport_width / camera.image_width, 0, 0};
  camera.pixel_delta_v = (Vec3){0, -viewport_height / camera.image_height, 0};

  Vec3 viewport_upper_left =
      vec3_sub(vec3_sub(vec3_sub(camera.center, (Vec3){0, 0, focal_length}),
                        half_viewport_u),
               half_viewport_v);

  camera.pixel00_loc = vec3_add(
      viewport_upper_left,
      vec3_scale(vec3_add(camera.pixel_delta_u, camera.pixel_delta_v), 0.5));

  camera.samples_per_pixel = SAMPLES_PER_PIXEL;
  camera.pixel_samples_scale = 1 / camera.samples_per_pixel;
  return camera;
}

static Color ray_color(Ray r, DynArray *hittable_world) {
  HitRecord rec;
  if (hittables_hit(hittable_world, r, interval_make(0, INFINITY), &rec)) {
    return vec3_scale(vec3_adds(rec.normal, 1), 0.5);
  }
  Vec3 unit_direction = vec3_normalized(r.direction);
  double a = (unit_direction.y + 1) * 0.5;
  return vec3_add((Vec3){1.0 - a, 1.0 - a, 1.0 - a},
                  (Vec3){0.5 * a, 0.7 * a, 1.0 * a});
}

// Construct a camera ray originating from the origin and directed at randomly
// sampled point around the pixel location i, j.
static Ray get_ray(const Camera *cam, int i, int j) {
  Vec3 offset = sample_square();
  Vec3 pixel_sample = vec3_add(
      cam->pixel00_loc, vec3_add(vec3_scale(cam->pixel_delta_u, i + offset.x),
                                 vec3_scale(cam->pixel_delta_v, j + offset.y)));

  return (Ray){.origin = cam->center,
               .direction = vec3_sub(pixel_sample, cam->center)};
}

void camera_render(const Camera *cam, DynArray *hittable_world,
                   FILE *out_file) {
  fprintf(out_file, "P3\n%d %d\n255\n", cam->image_width, cam->image_height);
  for (int j = 0; j < cam->image_height; j++) {
    for (int i = 0; i < cam->image_width; i++) {
      Vec3 pixel_color = vec3_zero();
      for(int sample=0; sample < cam->samples_per_pixel; sample++) {
        Ray r = get_ray(cam, i, j);
        pixel_color = vec3_add(pixel_color, ray_color(r, hittable_world));
      }
      write_color(out_file, vec3_divs(pixel_color, cam->samples_per_pixel));
    }
  }
}