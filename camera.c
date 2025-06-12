
#include "camera.h"
#include "core/color.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"

// Create a new camera instance
Camera camera_make(int image_width, double aspect_ratio) {
  Camera camera;
  camera.image_width = image_width;
  camera.aspect_ratio = aspect_ratio;
  camera.image_height = (int)(image_width / aspect_ratio);

  double viewport_height = 2.0;
  double viewport_width =
      viewport_height * (double)camera.image_width / camera.image_height;
  double focal_length = 1.0;

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

void camera_render(const Camera *cam, DynArray *hittable_world,
                   FILE *out_file) {
  fprintf(out_file, "P3\n%d %d\n255\n", cam->image_width, cam->image_height);
  for (int j = 0; j < cam->image_height; j++) {
    for (int i = 0; i < cam->image_width; i++) {
      Vec3 pixel_pos = vec3_add(cam->pixel00_loc,
                                vec3_add(vec3_scale(cam->pixel_delta_u, i),
                                         vec3_scale(cam->pixel_delta_v, j)));
      Ray r = (Ray){cam->center, vec3_sub(pixel_pos, cam->center)};
      Color pixel_color = ray_color(r, hittable_world);
      write_color(out_file, pixel_color);
    }
  }
}