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
  Vec3 lookfrom = {13, 2, 3};
  Vec3 lookat = {0, 0, -1};
  Vec3 vup = {0, 1, 0};
  double defocus_angle = 0.6;
  double focus_dist = 10;
  Camera cam =
      camera_make(WIDTH, ASPECT_RATIO, lookfrom, lookat, vup, vfov,
                  defocus_angle, focus_dist, SAMPLES_PER_PIXEL, MAX_DEPTH);

  Material *mat_ground =
      scene_add_material(&scene, lambertian_create((Vec3){0.5, 0.5, 0.5}));
  scene_add_obj(&scene, sphere_create((Vec3){0, -1000, 0}, 1000, mat_ground));

  // Add small spheres in a grid with random materials
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      double choose_mat = random_double();
      Vec3 center = {a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};
      Vec3 diff = vec3_sub(center, (Vec3){4, 0.2, 0});
      if (vec3_length(diff) > 0.9) {
        Material *sphere_material = NULL;
        if (choose_mat < 0.8) {
          // diffuse
          Vec3 albedo = vec3_mul(vec3_random(), vec3_random());
          sphere_material =scene_add_material(&scene, lambertian_create(albedo));
        } else if (choose_mat < 0.95) {
          // metal
          Vec3 albedo = vec3_random_bounded(0.5, 1.0);
          double fuzz = random_double_range(0, 0.5);
          sphere_material =scene_add_material(&scene, metal_create(albedo, fuzz));
        } else {
          // glass
          sphere_material = scene_add_material(&scene, dielectric_create(1.5));
        }
        scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));
      }
    }
  }

  // Add three large spheres
  Material *material1 = scene_add_material(&scene, dielectric_create(1.5));
  scene_add_obj(&scene, sphere_create((Vec3){0, 1, 0}, 1.0, material1));

  Material *material2 =
      scene_add_material(&scene, lambertian_create((Vec3){0.4, 0.2, 0.1}));
  scene_add_obj(&scene, sphere_create((Vec3){-4, 1, 0}, 1.0, material2));

  Material *material3 =
      scene_add_material(&scene, metal_create((Vec3){0.7, 0.6, 0.5}, 0.0));
  scene_add_obj(&scene, sphere_create((Vec3){4, 1, 0}, 1.0, material3));

  camera_render(&cam, scene.objects, out_file);

  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}