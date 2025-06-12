#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/color.h"
#include "core/dyn_array.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "core/generic_types.h"

#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/sphere.h"

#include "shared.h"

Color ray_color(Ray r, DynArray *hittable_world) {
  HitRecord rec;
  if (hittables_hit(hittable_world, r, interval_make(0, INFINITY), &rec)) {
    return vec3_scale(vec3_adds(rec.normal, 1), 0.5);
  }
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

  DynArray *hittable_world =
      dynarray_create(2, (GPrintFn)hittable_print, (GDestroyFn)hittable_destroy);
  Hittable *sphere1 = sphere_create((Vec3){0, 0, -1}, 0.5);
  Hittable *sphere2 = sphere_create((Vec3){0, -100.5, -1}, 100);
  dynarray_push(hittable_world, sphere2);
  dynarray_push(hittable_world, sphere1);

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
      Color pixel_color = ray_color(r, hittable_world);
      write_color(out_file, pixel_color);
    }
  }

  dynarray_destroy(hittable_world);
  fclose(out_file);
}