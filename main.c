#include "camera.h"
#include "core/color.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/obj_parser.h"
#include "core/ray.h"
#include "core/scene_parser.h"
#include "core/vec3.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/quad.h"
#include "hittable/rotate_y.h"
#include "hittable/sphere.h"
#include "hittable/translate.h"
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
  parse_scene(argv[1], &scene, &cam);

  camera_render(&cam, scene.objects, out_file);
  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}
