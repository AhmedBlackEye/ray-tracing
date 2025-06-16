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
#include "hittable/triangle_hittable.h"
#include "hittable/triangle_mesh.h"
#include "hittable/triangle_raw.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/material.h"
#include "material/metal.h"
#include "scene.h"
#include "shared.h"

// Add this function to your main.c (or the test functions from previous
// artifact)

void create_simple_table(Scene *scene) {
  // Wood-like brown material
  Material *wood_material =
      scene_add_material(scene, lambertian_create((Vec3){0.6, 0.4, 0.2}));

  Hittable *table_mesh = mesh_create(wood_material);

  // Table dimensions
  double table_width = 3.0;  // X dimension
  double table_depth = 2.0;  // Z dimension
  double table_height = 1.5; // Y dimension
  double table_thickness = 0.1;
  double leg_thickness = 0.2;

  // Table position (centered at origin, moved back)
  Vec3 center = {0, 0, -4};

  // === TABLE TOP ===
  Vec3 top_corners[4] = {
      {center.x - table_width / 2, center.y + table_height,
       center.z - table_depth / 2}, // front-left
      {center.x + table_width / 2, center.y + table_height,
       center.z - table_depth / 2}, // front-right
      {center.x + table_width / 2, center.y + table_height,
       center.z + table_depth / 2}, // back-right
      {center.x - table_width / 2, center.y + table_height,
       center.z + table_depth / 2} // back-left
  };

  Vec3 bottom_corners[4] = {
      {center.x - table_width / 2, center.y + table_height - table_thickness,
       center.z - table_depth / 2},
      {center.x + table_width / 2, center.y + table_height - table_thickness,
       center.z - table_depth / 2},
      {center.x + table_width / 2, center.y + table_height - table_thickness,
       center.z + table_depth / 2},
      {center.x - table_width / 2, center.y + table_height - table_thickness,
       center.z + table_depth / 2}};

  // Top surface (2 triangles)
  mesh_add_triangle(table_mesh, top_corners[0], top_corners[1], top_corners[2]);
  mesh_add_triangle(table_mesh, top_corners[0], top_corners[2], top_corners[3]);

  // Bottom surface (2 triangles) - facing downward
  mesh_add_triangle(table_mesh, bottom_corners[0], bottom_corners[2],
                    bottom_corners[1]);
  mesh_add_triangle(table_mesh, bottom_corners[0], bottom_corners[3],
                    bottom_corners[2]);

  // Side edges (8 triangles - 2 per side)
  for (int i = 0; i < 4; i++) {
    int next = (i + 1) % 4;
    // Side face
    mesh_add_triangle(table_mesh, top_corners[i], top_corners[next],
                      bottom_corners[next]);
    mesh_add_triangle(table_mesh, top_corners[i], bottom_corners[next],
                      bottom_corners[i]);
  }

  // === TABLE LEGS ===
  double leg_positions[4][2] = {
      {-table_width / 2 + leg_thickness,
       -table_depth / 2 + leg_thickness}, // front-left
      {table_width / 2 - leg_thickness,
       -table_depth / 2 + leg_thickness}, // front-right
      {table_width / 2 - leg_thickness,
       table_depth / 2 - leg_thickness}, // back-right
      {-table_width / 2 + leg_thickness,
       table_depth / 2 - leg_thickness} // back-left
  };

  for (int leg = 0; leg < 4; leg++) {
    double x = center.x + leg_positions[leg][0];
    double z = center.z + leg_positions[leg][1];

    // Leg vertices (rectangular leg)
    Vec3 leg_top[4] = {
        {x - leg_thickness / 2, center.y + table_height - table_thickness,
         z - leg_thickness / 2},
        {x + leg_thickness / 2, center.y + table_height - table_thickness,
         z - leg_thickness / 2},
        {x + leg_thickness / 2, center.y + table_height - table_thickness,
         z + leg_thickness / 2},
        {x - leg_thickness / 2, center.y + table_height - table_thickness,
         z + leg_thickness / 2}};

    Vec3 leg_bottom[4] = {
        {x - leg_thickness / 2, center.y, z - leg_thickness / 2},
        {x + leg_thickness / 2, center.y, z - leg_thickness / 2},
        {x + leg_thickness / 2, center.y, z + leg_thickness / 2},
        {x - leg_thickness / 2, center.y, z + leg_thickness / 2}};

    // Leg sides (8 triangles per leg)
    for (int i = 0; i < 4; i++) {
      int next = (i + 1) % 4;
      mesh_add_triangle(table_mesh, leg_top[i], leg_bottom[next],
                        leg_top[next]);
      mesh_add_triangle(table_mesh, leg_top[i], leg_bottom[i],
                        leg_bottom[next]);
    }
  }

  scene_add_obj(scene, table_mesh);
  printf("Created simple table\n");
}
void create_round_table(Scene *scene) {
  Material *marble_material =
      scene_add_material(scene, lambertian_create((Vec3){0.9, 0.9, 0.8}));

  Hittable *table_mesh = mesh_create(marble_material);

  Vec3 center = {0, 0, -3};
  double radius = 1.5;
  double height = 1.2;
  double thickness = 0.08;
  int segments = 8; // 8-sided "round" table

  // Create octagonal top
  Vec3 top_verts[8];
  Vec3 bottom_verts[8];

  for (int i = 0; i < segments; i++) {
    double angle = 2.0 * PI * i / segments;
    double x = center.x + radius * cos(angle);
    double z = center.z + radius * sin(angle);

    top_verts[i] = (Vec3){x, center.y + height, z};
    bottom_verts[i] = (Vec3){x, center.y + height - thickness, z};
  }

  // Top surface (triangular fan from center)
  Vec3 top_center = {center.x, center.y + height, center.z};
  for (int i = 0; i < segments; i++) {
    int next = (i + 1) % segments;
    mesh_add_triangle(table_mesh, top_center, top_verts[next], top_verts[i]);
  }

  // Bottom surface
  Vec3 bottom_center = {center.x, center.y + height - thickness, center.z};
  for (int i = 0; i < segments; i++) {
    int next = (i + 1) % segments;
    mesh_add_triangle(table_mesh, bottom_center, bottom_verts[i],
                      bottom_verts[next]);
  }

  // Side edges
  for (int i = 0; i < segments; i++) {
    int next = (i + 1) % segments;
    mesh_add_triangle(table_mesh, top_verts[i], top_verts[next],
                      bottom_verts[next]);
    mesh_add_triangle(table_mesh, top_verts[i], bottom_verts[next],
                      bottom_verts[i]);
  }

  // Single central pedestal leg
  double leg_radius = 0.3;
  int leg_segments = 6;

  Vec3 leg_top_verts[6];
  Vec3 leg_bottom_verts[6];

  for (int i = 0; i < leg_segments; i++) {
    double angle = 2.0 * PI * i / leg_segments;
    double x = center.x + leg_radius * cos(angle);
    double z = center.z + leg_radius * sin(angle);

    leg_top_verts[i] = (Vec3){x, center.y + height - thickness, z};
    leg_bottom_verts[i] = (Vec3){x, center.y, z};
  }

  // Leg sides
  for (int i = 0; i < leg_segments; i++) {
    int next = (i + 1) % leg_segments;
    mesh_add_triangle(table_mesh, leg_top_verts[i], leg_bottom_verts[next],
                      leg_top_verts[next]);
    mesh_add_triangle(table_mesh, leg_top_verts[i], leg_bottom_verts[i],
                      leg_bottom_verts[next]);
  }

  scene_add_obj(scene, table_mesh);
  printf("Created round table\n");
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
  create_round_table(&scene);
  create_simple_table(&scene);
  // Add small spheres in a grid with random materials

  camera_render(&cam, scene.objects, out_file);

  scene_destroy(&scene);
  fclose(out_file);
  return 0;
}
