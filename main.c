#include "camera.h"
#include "core/color.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/obj_parser.h"
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

void debug_obj_bounds(const char *filename) {
  printf("=== Analyzing OBJ file: %s ===\n", filename);

  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("❌ Cannot open file: %s\n", filename);
    return;
  }

  Vec3 min_bounds = {INFINITY, INFINITY, INFINITY};
  Vec3 max_bounds = {-INFINITY, -INFINITY, -INFINITY};
  int vertex_count = 0;
  int face_count = 0;

  char line[512];
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'v' && line[1] == ' ') {
      Vec3 vertex;
      if (sscanf(line + 2, "%lf %lf %lf", &vertex.x, &vertex.y, &vertex.z) ==
          3) {
        vertex_count++;

        // Update bounds
        if (vertex.x < min_bounds.x)
          min_bounds.x = vertex.x;
        if (vertex.y < min_bounds.y)
          min_bounds.y = vertex.y;
        if (vertex.z < min_bounds.z)
          min_bounds.z = vertex.z;
        if (vertex.x > max_bounds.x)
          max_bounds.x = vertex.x;
        if (vertex.y > max_bounds.y)
          max_bounds.y = vertex.y;
        if (vertex.z > max_bounds.z)
          max_bounds.z = vertex.z;
      }
    } else if (line[0] == 'f' && line[1] == ' ') {
      face_count++;
    }
  }
  fclose(file);

  if (vertex_count == 0) {
    printf("❌ No vertices found in file\n");
    return;
  }

  Vec3 center = {(min_bounds.x + max_bounds.x) / 2.0,
                 (min_bounds.y + max_bounds.y) / 2.0,
                 (min_bounds.z + max_bounds.z) / 2.0};

  Vec3 size = {max_bounds.x - min_bounds.x, max_bounds.y - min_bounds.y,
               max_bounds.z - min_bounds.z};

  printf("📊 Object Statistics:\n");
  printf("  Vertices: %d\n", vertex_count);
  printf("  Faces: %d\n", face_count);
  printf("\n");
  printf("📐 Bounding Box:\n");
  printf("  Min: (%.3f, %.3f, %.3f)\n", min_bounds.x, min_bounds.y,
         min_bounds.z);
  printf("  Max: (%.3f, %.3f, %.3f)\n", max_bounds.x, max_bounds.y,
         max_bounds.z);
  printf("  Center: (%.3f, %.3f, %.3f)\n", center.x, center.y, center.z);
  printf("  Size: (%.3f, %.3f, %.3f)\n", size.x, size.y, size.z);
  printf("\n");
  printf("💡 Positioning Tips:\n");

  if (center.y < 0) {
    printf("  ⚠️  Object center is below Y=0, try: position 0 %.1f 0\n",
           -center.y);
  } else if (center.y > 1) {
    printf("  ⚠️  Object center is high (Y=%.1f), try: position 0 0 0\n",
           center.y);
  } else {
    printf("  ✅ Object Y position looks reasonable\n");
  }

  if (size.x > 10 || size.y > 10 || size.z > 10) {
    double scale_factor = 2.0 / fmax(fmax(size.x, size.y), size.z);
    printf("  ⚠️  Object is large, try: scale %.2f %.2f %.2f\n", scale_factor,
           scale_factor, scale_factor);
  } else if (size.x < 0.1 || size.y < 0.1 || size.z < 0.1) {
    double scale_factor = 2.0 / fmax(fmax(size.x, size.y), size.z);
    printf("  ⚠️  Object is tiny, try: scale %.1f %.1f %.1f\n", scale_factor,
           scale_factor, scale_factor);
  } else {
    printf("  ✅ Object size looks reasonable\n");
  }

  printf("\n");
}

// Quick diagnostic: check if object is visible from camera
void diagnose_visibility(Vec3 camera_pos, Vec3 object_pos, Vec3 object_size) {
  printf("=== Visibility Diagnosis ===\n");

  double distance = vec3_length(vec3_sub(object_pos, camera_pos));
  double max_object_size =
      fmax(fmax(object_size.x, object_size.y), object_size.z);

  printf("Camera position: (%.1f, %.1f, %.1f)\n", camera_pos.x, camera_pos.y,
         camera_pos.z);
  printf("Object position: (%.1f, %.1f, %.1f)\n", object_pos.x, object_pos.y,
         object_pos.z);
  printf("Distance: %.1f units\n", distance);
  printf("Object max size: %.1f units\n", max_object_size);

  if (distance < max_object_size * 2) {
    printf("⚠️  Camera might be too close! Try moving camera further away.\n");
  } else if (distance > max_object_size * 50) {
    printf("⚠️  Camera might be too far! Object could appear very small.\n");
  } else {
    printf("✅ Distance looks reasonable.\n");
  }

  if (object_pos.y < -max_object_size) {
    printf("⚠️  Object might be underground! Check Y position.\n");
  }

  if (camera_pos.y < object_pos.y + max_object_size) {
    printf("💡 Try raising camera Y position to see object better.\n");
  }
}

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
  // Add small spheres in a grid with random materials
  // for (int a = -5; a < 5; a++) {
  //   for (int b = -5; b < 5; b++) {
  //     double choose_mat = random_double();
  //     Vec3 center = {a + 0.9 * random_double(), 0.2, b + 0.9 *
  //     random_double()}; Vec3 diff = vec3_sub(center, (Vec3){4, 0.2, 0});

  //     if (vec3_length(diff) > 0.9) {
  //       Material *sphere_material = NULL;

  //       if (choose_mat < 0.8) {
  //         // diffuse
  //         Vec3 albedo = vec3_mul(vec3_random(), vec3_random());
  //         sphere_material =
  //             scene_add_material(&scene, lambertian_create(albedo));

  //         // ALL diffuse spheres move
  //         // Vec3 center2 =
  //         //     vec3_add(center, (Vec3){0, random_double_range(0, 0.5), 0});
  //         scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));
  //       } else if (choose_mat < 0.95) {
  //         // metal
  //         Vec3 albedo = vec3_random_bounded(0.5, 1.0);
  //         double fuzz = random_double_range(0, 0.5);
  //         sphere_material =
  //             scene_add_material(&scene, metal_create(albedo, fuzz));
  //         scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));

  //       } else {
  //         // glass
  //         sphere_material = scene_add_material(&scene,
  //         dielectric_create(1.5)); scene_add_obj(&scene,
  //         sphere_create(center, 0.2, sphere_material));
  //       }
  //     }
  //   }
  // }

  Hittable *temp_world = hittablelist_empty();
  hittablelist_add(temp_world, bvhnode_create(scene.objects));
  scene.objects = temp_world;
  debug_obj_bounds("models/cow.obj");
  camera_render(&cam, scene.objects, out_file);
  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}
