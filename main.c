#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "core/color.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/ray.h"
#include "core/scene_parser.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/quad.h"
#include "hittable/sphere.h"
#include "hittable/triangle.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/material.h"
#include "material/metal.h"
#include "scene.h"
#include "shared.h"

#define WIDTH 400
#define ASPECT_RATIO (16.0 / 9.0)
#define SAMPLES_PER_PIXEL 10
#define MAX_DEPTH 50
#define VFOV 90
#define DEFOCUS_ANGLE 0
#define FOCUS_DIST 10
#define LOOKFROM (vec3_zero())
#define LOOKAT ((Vec3){0, 0, -1})
#define VUP ((Vec3){0, 1, 0})

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <scene_file> <output_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *out_file = fopen(argv[2], "w");
  if (!out_file) {
    perror("Failed to open input file");
    return EXIT_FAILURE;
  }

  Scene scene = scene_create();

  double vfov = 20;
  Vec3 lookfrom = {-2, 2, 1};
  Vec3 lookat = {0, 0, -1};
  Vec3 vup = {0, 1, 0};
  double defocus_angle = 10.0;
  double focus_dist = 3.4;
  Camera cam =
      camera_make(WIDTH, ASPECT_RATIO, lookfrom, lookat, vup, vfov,
                  defocus_angle, focus_dist, SAMPLES_PER_PIXEL, MAX_DEPTH);

  Material *mat_ground =
      scene_add_material(&scene, lambertian_create((Color){0.8, 0.5, 0.6}));
  Material *mat_center =
      scene_add_material(&scene, lambertian_create((Color){0.15, 0.45, 0.4}));
  Material *mat_left = scene_add_material(&scene, dielectric_create(1.5));
  Material *mat_bubble = scene_add_material(&scene, dielectric_create(1 / 1.5));
  Material *mat_right =
      scene_add_material(&scene, metal_create((Color){0.8, 0.6, 0.2}, 1.0));

  scene_add_obj(&scene,
                sphere_create((Vec3){0.0, -100.5, -1}, 100.0, mat_ground));
  scene_add_obj(&scene, sphere_create((Vec3){0.0, 0.0, -1.2}, 0.5, mat_center));
  scene_add_obj(&scene, sphere_create((Vec3){-1.0, 0.0, -1.0}, 0.5, mat_left));
  scene_add_obj(&scene,
                sphere_create((Vec3){-1.0, 0.0, -1.0}, 0.3, mat_bubble));
  scene_add_obj(&scene, sphere_create((Vec3){1.0, 0.0, -1.0}, 0.5, mat_right));

  camera_render(&cam, scene.objects, out_file);

  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}