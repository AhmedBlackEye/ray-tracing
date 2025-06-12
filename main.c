#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/vec3.h"
#include "core/color.h"
#include "core/ray.h"

#include "hittable/hittable.h"
#include "hittable/sphere.h"


#define ASPECT_RATIO (16.0 / 9.0)

#define WIDTH 400
#define HEIGHT ((int)(WIDTH / ASPECT_RATIO))

#define VIEWPORT_HEIGHT 2.0
#define VIEWPORT_WIDTH (VIEWPORT_HEIGHT * (double)WIDTH / HEIGHT)

#define FOCAL_LEN 1.0

Color ray_color(Ray r) {
  Hittable *sphere = sphere_create((Vec3){0, 0, -1}, 0.5);
  HitRecord rec = hitrecord_zero();
  HitRecord *rec_ptr;
  if (sphere->hit(sphere, r, 0, 10000, rec_ptr)) {
    sphere->destroy(sphere);
    return vec3_scale(vec3_adds(rec_ptr->normal, 1), 0.5);
  }
  sphere->destroy(sphere);
  Vec3 unit_direction = vec3_normalized(r.direction);
  double a = (unit_direction.y + 1) * 0.5;
  return vec3_add((Vec3){1.0 - a, 1.0 - a, 1.0 - a},
                  (Vec3){0.5 * a, 0.7 * a, 1.0 * a});
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file> [output_file]\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *out_file = fopen(argv[1], "w");
  if (!out_file) {
    perror("Failed to open input file");
    return EXIT_FAILURE;
  }

  Vec3 camera_center = vec3_zero();

  Vec3 half_viewport_u = (Vec3){VIEWPORT_WIDTH / 2, 0, 0};
  Vec3 half_viewport_v = (Vec3){0, -VIEWPORT_HEIGHT / 2, 0};

  Vec3 pixel_delta_u = (Vec3){VIEWPORT_WIDTH / WIDTH, 0, 0};
  Vec3 pixel_delta_v = (Vec3){0, -VIEWPORT_HEIGHT / HEIGHT, 0};

  Vec3 focal = (Vec3){0, 0, FOCAL_LEN};
  Vec3 offset = vec3_add(half_viewport_u, half_viewport_v);
  Vec3 viewport_upper_left = vec3_sub(camera_center, vec3_add(focal, offset));

  Vec3 pixel00_loc =
      vec3_add(viewport_upper_left,
               vec3_scale(vec3_add(pixel_delta_u, pixel_delta_v), 0.5));

  fprintf(out_file, "P3\n%d %d\n255\n", WIDTH, HEIGHT);
  for (int j = 0; j < HEIGHT; j++) {
    for (int i = 0; i < WIDTH; i++) {
      Vec3 pixel_pos =
          vec3_add(pixel00_loc, vec3_add(vec3_scale(pixel_delta_u, i),
                                         vec3_scale(pixel_delta_v, j)));
      Ray r = (Ray){camera_center, vec3_sub(pixel_pos, camera_center)};
      // Color pixel = (Color){(double)i / (WIDTH-1), (double)j / (HEIGHT-1),
      // 0};
      write_color(out_file, ray_color(r));
    }
  }
  fclose(out_file);
}