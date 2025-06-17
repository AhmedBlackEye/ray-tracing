#include "camera.h"
#include "core/color.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/ray.h"
#include "core/scene_parser.h"
#include "core/vec3.h"
#include "hittable/bvh_node.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/quad.h"
#include "hittable/sphere.h"
#include "hittable/triangle_hittable.h"
#include "hittable/triangle_mesh.h"
#include "hittable/triangle_raw.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/material.h"
#include "material/metal.h"
#include "scene.h"
#include "shared.h"
#include "texture/checkered.h"
#include "texture/solid_color.h"
#include "texture/texture.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
  Camera cam;

  // Material *glass =
  //     scene_add_material(&scene, metal_create((Color){1, 1, 1}, 0.0));
  // scene_add_obj(&scene, sphere_create((Vec3){0, -1000, 0}, 1000, glass));
  parse_scene(argv[1], &scene, &cam);
  create_round_table(&scene);
  create_simple_table(&scene);
  // Add small spheres in a grid with random materials
  for (int a = -5; a < 5; a++) {
    for (int b = -5; b < 5; b++) {
      double choose_mat = random_double();
      Vec3 center = {a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};
      Vec3 diff = vec3_sub(center, (Vec3){4, 0.2, 0});

      if (vec3_length(diff) > 0.9) {
        Material *sphere_material = NULL;

        if (choose_mat < 0.8) {
          // diffuse
          Vec3 albedo = vec3_mul(vec3_random(), vec3_random());
          sphere_material =
              scene_add_material(&scene, lambertian_create(albedo));

          // ALL diffuse spheres move
          // Vec3 center2 =
          //     vec3_add(center, (Vec3){0, random_double_range(0, 0.5), 0});
          scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          Vec3 albedo = vec3_random_bounded(0.5, 1.0);
          double fuzz = random_double_range(0, 0.5);
          sphere_material =
              scene_add_material(&scene, metal_create(albedo, fuzz));
          scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));

        } else {
          // glass
          sphere_material = scene_add_material(&scene, dielectric_create(1.5));
          scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));
        }
      }
    }
  }

  Hittable *temp_world = hittablelist_empty();
  hittablelist_add(temp_world, bvhnode_create(scene.objects));
  scene.objects = temp_world;

  camera_render(&cam, scene.objects, out_file);
  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}
