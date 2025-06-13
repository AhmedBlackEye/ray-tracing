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
#include "core/scene_parser.h"

#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/sphere.h"
#include "hittable/triangle.h"

#include "shared.h"
#include "camera.h"

#define ASPECT_RATIO (16.0 / 9.0)
#define WIDTH 400

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

  DynArray *hittable_world =
      dynarray_create(2, (GPrintFn)hittable_print, (GDestroyFn)hittable_destroy);

  parse_scene(argv[1], hittable_world, out_file);    

  dynarray_destroy(hittable_world);
  fclose(out_file);
  return 0;
}