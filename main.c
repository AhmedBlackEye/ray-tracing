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
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }
    
    Scene scene = scene_create();
    Camera cam;
    parse_scene(argv[1], &scene, &cam);

    Material *red = scene_add_material(&scene, lambertian_create((Vec3){0.8, 0.2, 0.2}));
    Material *blue = scene_add_material(&scene, lambertian_create((Vec3){0.2, 0.2, 0.8}));

    // Normal sphere (red)
    Hittable *normal_sphere = sphere_create((Vec3){-2, 0, -5}, 1.0, red);
    scene_add_obj(&scene, normal_sphere);

    // Rotated sphere (blue) - should look identical since spheres are symmetric
    Vec3 v0 = {-1, -1, -3};
    Vec3 v1 = {1, -1, -3}; 
    Vec3 v2 = {0, 1, -3};

    Hittable *triangle = triangle_create(v0, v1, v2, red);
    scene_add_obj(&scene, triangle);
    
    camera_render(&cam, scene.objects, out_file);
    scene_destroy(&scene);
    fclose(out_file);
    return 0;
}
