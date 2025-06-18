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
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/material.h"
#include "material/metal.h"
#include "hittable/rotate_y.h"
#include "hittable/translate.h"
#include "texture/texture.h"
#include "texture/solid_color.h"
#include "texture/checkered.h"
#include "scene.h"
#include "shared.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <scene_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *out_file = fopen(argv[2], "w");
    if (!out_file) {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    Scene scene = scene_create();
    Camera cam;
    parse_scene(argv[1], &scene, &cam);

    // Create materials
    Material *red = scene_add_material(&scene, lambertian_create((Vec3){0.8, 0.2, 0.2}));
    Material *green = scene_add_material(&scene, lambertian_create((Vec3){0.2, 0.8, 0.2}));
    Material *blue = scene_add_material(&scene, lambertian_create((Vec3){0.2, 0.2, 0.8}));
    
    // Create a box manually (avoid dynarray ownership issues)
    Vec3 box_min = {-1, -1, -3};
    Vec3 box_max = {1, 1, -5};
    
    Vec3 min = (Vec3){fmin(box_min.x, box_max.x), fmin(box_min.y, box_max.y), fmin(box_min.z, box_max.z)};
    Vec3 max = (Vec3){fmax(box_min.x, box_max.x), fmax(box_min.y, box_max.y), fmax(box_min.z, box_max.z)};
    Vec3 dx = (Vec3){max.x - min.x, 0, 0};
    Vec3 dy = (Vec3){0, max.y - min.y, 0};
    Vec3 dz = (Vec3){0, 0, max.z - min.z};
    
    // Create and add all 6 box faces directly to scene
    scene_add_obj(&scene, quad_create((Vec3){min.x, min.y, max.z}, dx, dy, red));           // front
    scene_add_obj(&scene, quad_create((Vec3){max.x, min.y, max.z}, vec3_negate(dz), dy, red)); // right  
    scene_add_obj(&scene, quad_create((Vec3){max.x, min.y, min.z}, vec3_negate(dx), dy, red)); // back
    scene_add_obj(&scene, quad_create((Vec3){min.x, min.y, min.z}, dz, dy, red));           // left
    scene_add_obj(&scene, quad_create((Vec3){min.x, max.y, max.z}, dx, vec3_negate(dz), red)); // top
    scene_add_obj(&scene, quad_create((Vec3){min.x, min.y, min.z}, dx, dz, red));            // bottom
    
    // Create a single quad and rotate it
    Hittable *test_quad = quad_create(
        (Vec3){-0.5, -2, -4},  // Q (bottom left corner)
        (Vec3){1, 0, 0},       // u (width vector)
        (Vec3){0, 1, 0},       // v (height vector)  
        green
    );
    
    // Rotate the quad by 45 degrees
    Hittable *rotated_quad = rotate_y_create(test_quad, 45.0);
    scene_add_obj(&scene, rotated_quad);
    
    // Create another quad and translate it
    Hittable *small_quad = quad_create(
        (Vec3){-0.3, 0, -3.5},
        (Vec3){0.6, 0, 0},
        (Vec3){0, 0.6, 0},
        blue
    );
    
    Vec3 translate_offset = {2, 1, 0};
    Hittable *translated_quad = translate_create(small_quad, translate_offset);
    scene_add_obj(&scene, translated_quad);

    camera_render(&cam, scene.objects, out_file);

    
    scene_destroy(&scene);
    fclose(out_file);
    return 0;
}
