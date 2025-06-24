#include "camera.h"
#include "core/color.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/obj_parser.h"
#include "core/ray.h"
#include "core/scene_parser.h"
#include "core/vec3.h"
#include "hittable/bvh_node.h" // Add this include
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
#include <string.h> // ADD THIS LINE

int main(int argc, char **argv) {
  printf("=== RAYTRACER STARTING ===\n");

  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Usage: %s <scene_file> <output_file> [--no-bvh]\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  bool use_bvh = true;
  if (argc == 4 && strcmp(argv[3], "--no-bvh") == 0) {
    use_bvh = false;
    printf("BVH acceleration disabled\n");
  }

  printf("Opening output file: %s\n", argv[2]);
  FILE *out_file = fopen(argv[2], "w");
  if (!out_file) {
    perror("Failed to open output file");
    return EXIT_FAILURE;
  }
  printf("Output file opened successfully\n");

  printf("Creating scene...\n");
  Scene scene = scene_create();
  printf("Scene created\n");

  Camera cam;
  printf("Parsing scene file: %s\n", argv[1]);
  parse_scene(argv[1], &scene, &cam);
  printf("Scene parsed successfully\n");

  // Check if we have objects to render
  DynArray *objects_array = (DynArray *)scene.objects->data;
  int object_count = dynarray_size(objects_array);
  printf("Scene contains %d objects\n", object_count);

  if (object_count == 0) {
    printf("Warning: No objects in scene to render\n");
    camera_render(&cam, scene.objects, out_file);
  } else if (!use_bvh) {
    printf("Rendering %d objects without BVH acceleration...\n", object_count);
    camera_render(&cam, scene.objects, out_file);
  } else {
    printf("Building BVH for %d objects...\n", object_count);

    // Create BVH acceleration structure
    Hittable *bvh = bvhnode_create(scene.objects);

    if (bvh) {
      printf("BVH built successfully! Starting render...\n");
      camera_render(&cam, bvh, out_file);
      printf("Rendering complete! Starting BVH cleanup...\n");

      // CRITICAL: Clean up BVH BEFORE scene destruction
      bvh->destroy(bvh);
      printf("BVH destroyed successfully\n");
    } else {
      printf("Failed to create BVH, falling back to linear rendering\n");
      camera_render(&cam, scene.objects, out_file);
    }
  }

  printf("Closing output file...\n");
  fclose(out_file);
  printf("Output file closed\n");

  printf("Destroying scene...\n");
  scene_destroy(&scene);
  printf("Scene destroyed successfully\n");

  printf("=== RAYTRACER COMPLETED ===\n");
  return 0;
}