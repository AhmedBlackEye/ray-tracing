
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
    
    Color white = {0.9, 0.9, 0.9};
    Color black = {0.1, 0.1, 0.1};
    Color red = {0.8, 0.2, 0.2};
    Color blue = {0.2, 0.2, 0.8};
    Color green = {0.2, 0.8, 0.2};
    
    Checkered* checker_bw = checkered_create_colors(0.32, &white, &black);
    Checkered* checker_rb = checkered_create_colors(0.5, &red, &blue);
    Checkered* checker_gw = checkered_create_colors(0.5, &green, &white);
    
    Material* checker_mat1 = lambertian_create_texture((Texture*)checker_bw);
    Material* checker_mat2 = lambertian_create_texture((Texture*)checker_rb);
    Material* checker_mat3 = lambertian_create_texture((Texture*)checker_gw);
    
    scene_add_obj(&scene, sphere_create((Vec3){0, 1, 0}, 1.0, 
                  scene_add_material(&scene, checker_mat1)));
    scene_add_obj(&scene, sphere_create((Vec3){-4, 1, 0}, 1.0, 
                  scene_add_material(&scene, checker_mat2))); 
    scene_add_obj(&scene, sphere_create((Vec3){4, 1, 0}, 1.0, 
                  scene_add_material(&scene, checker_mat3)));

    Color ground_green = {0.2, 0.3, 0.1};
    Color ground_white = {0.9, 0.9, 0.9};
    Checkered* ground_checker = checkered_create_colors(0.32, &ground_green, &ground_white);

    Material* ground_material = lambertian_create_texture((Texture*)ground_checker);

    scene_add_obj(&scene, sphere_create((Vec3){0, -1000, 0}, 1000.0, 
                  scene_add_material(&scene, ground_material)));
    
    // Add small spheres in a grid with random materials (including checkers!)
    for (int a = -5; a < 5; a++) {
        for (int b = -5; b < 5; b++) {
            double choose_mat = random_double();
            Vec3 center = {a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};
            Vec3 diff = vec3_sub(center, (Vec3){4, 0.2, 0});
            
            if (vec3_length(diff) > 0.9) {
                Material *sphere_material = NULL;
                
                if (choose_mat < 0.5) {
                    // diffuse solid color
                    Vec3 albedo = vec3_mul(vec3_random(), vec3_random());
                    sphere_material = scene_add_material(&scene, lambertian_create(albedo));
                } else if (choose_mat < 0.7) {
                    // checkered diffuse - add some variety!
                    Color c1 = vec3_random();
                    Color c2 = vec3_random(); 
                    double scale = random_double_range(0.1, 1.0);
                    Checkered* random_checker = checkered_create_colors(scale, &c1, &c2);
                    sphere_material = scene_add_material(&scene, 
                                      lambertian_create_texture((Texture*)random_checker));
                } else if (choose_mat < 0.85) {
                    // metal
                    Vec3 albedo = vec3_random_bounded(0.5, 1.0);
                    double fuzz = random_double_range(0, 0.5);
                    sphere_material = scene_add_material(&scene, metal_create(albedo, fuzz));
                } else {
                    // glass
                    sphere_material = scene_add_material(&scene, dielectric_create(1.5));
                }
                
                scene_add_obj(&scene, sphere_create(center, 0.2, sphere_material));
            }
        }
    }
    
    camera_render(&cam, scene.objects, out_file);
    scene_destroy(&scene);
    fclose(out_file);
    return 0;
}
