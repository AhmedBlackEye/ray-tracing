#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "core/vec3.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "hittable/sphere.h"
#include "hittable/plane.h"
#include "hittable/triangle.h"
#include "hittable/quad.h"
#include "material/material.h"    
#include "material/lambertian.h"  
#include "material/metal.h"
#include "camera.h"
#include "scene_parser.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 20
#define MAX_MATERIALS 20

typedef enum {
    TOPLEVEL_STATE,
    CAMERA_STATE,
    MATERIAL_STATE,
    SPHERE_STATE,
    PLANE_STATE,
    TRIANGLE_STATE,
    QUAD_STATE
} ParserState;

static int tokenize(char *line, char *tokens[]) {
    int num_toks = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, " \t\n", &saveptr);
    
    while (token && num_toks < MAX_TOKENS) {
        tokens[num_toks++] = token;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    return num_toks;
}

void parse_scene(const char *filename, DynArray *hittable_world, FILE *out_file) {
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    ParserState state = TOPLEVEL_STATE;

    char line[MAX_LINE_LENGTH];
    char *tokens[MAX_TOKENS];

    Vec3 center;
    double radius;

    Vec3 normal;
    Vec3 point;

    double aspect_ratio_width;
    double aspect_ratio_height;
    double aspect_ratio;
    int width;

    char *mat_names[MAX_MATERIALS];
    Material *mats[MAX_MATERIALS];
    int num_mats = 0;
    Material *current_mat;

    char  mat_name[32];
    char  mat_type[32];
    Vec3  color;
    double fuzz;

    Vec3 v0;
    Vec3 v1;
    Vec3 v2;

    Vec3 Q;
    Vec3 u;
    Vec3 v;

    bool make_camera = false;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (line[0] == '\n') {
            continue;
        }

        if (strchr(line, '}')) {
            switch (state) {
                case MATERIAL_STATE:
                    Material *mat;
                    if (strcmp(mat_type, "lambertian") == 0) {
                        mat = lambertian_create(color);
                    }
                    else if (strcmp(mat_type, "metal") == 0) {
                        mat = metal_create(color, fuzz);
                    }
                    mat_names[num_mats] = strdup(mat_name);
                    mats[num_mats++] = mat;
                    break;
                case SPHERE_STATE:
                    dynarray_push(hittable_world, sphere_create(center, radius, current_mat));
                    break;
                case PLANE_STATE:
                    dynarray_push(hittable_world, plane_create(point, normal, current_mat));
                    break;
                case TRIANGLE_STATE:
                    dynarray_push(hittable_world, triangle_create(v0, v1, v2, current_mat));
                    break;
                case QUAD_STATE:
                    dynarray_push(hittable_world, quad_create(Q, u, v, current_mat));
                    break;
                case CAMERA_STATE:
                    make_camera = true;
                    break;
                case TOPLEVEL_STATE:
                    break;
            }
            state = TOPLEVEL_STATE;
            continue;
        }

        int num_toks = tokenize(line, tokens);
        if (num_toks == 0) continue;

        if (num_toks == 2 && strcmp(tokens[0], "material") == 0) {
            for (int i = 0; i < num_mats; i++) {
                if (strcmp(tokens[1], mat_names[i]) == 0) {
                    current_mat = mats[i];
                    break;
                }
            }
            continue; 
        }
        if (state == TOPLEVEL_STATE) {
            if (strcmp(tokens[0], "camera") == 0) {
                state = CAMERA_STATE;
            }
            else if (strcmp(tokens[0], "material") == 0) {
                state = MATERIAL_STATE;
            }
            else if (strcmp(tokens[0], "sphere") == 0) {
                state = SPHERE_STATE;
            }
            else if (strcmp(tokens[0], "plane") == 0) {
                state = PLANE_STATE;
            }
            else if (strcmp(tokens[0], "triangle") == 0) {
                state = TRIANGLE_STATE;
            }
            else if (strcmp(tokens[0], "quad") == 0) {
                state = QUAD_STATE;
            }
        }
        else {
            switch (state) {
                case SPHERE_STATE:
                    if (strcmp(tokens[0], "center") == 0 && num_toks == 4) {
                        center.x = atof(tokens[1]);
                        center.y = atof(tokens[2]);
                        center.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "radius") == 0 && num_toks == 2) {
                        radius = atof(tokens[1]);
                    }
                    break;
                case PLANE_STATE:
                    if (strcmp(tokens[0], "point") == 0 && num_toks == 4) {
                        point.x = atof(tokens[1]);
                        point.y = atof(tokens[2]);
                        point.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "normal") == 0 && num_toks == 4) {
                        normal.x = atof(tokens[1]);
                        normal.y = atof(tokens[2]);
                        normal.z = atof(tokens[3]);
                    }
                    break;
                case TRIANGLE_STATE:
                    if (strcmp(tokens[0], "v0") == 0 && num_toks == 4) {
                        v0.x = atof(tokens[1]);
                        v0.y = atof(tokens[2]);
                        v0.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "v1") == 0 && num_toks == 4) {
                        v1.x = atof(tokens[1]);
                        v1.y = atof(tokens[2]);
                        v1.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "v2") == 0 && num_toks == 4) {
                        v2.x = atof(tokens[1]);
                        v2.y = atof(tokens[2]);
                        v2.z = atof(tokens[3]);
                    }
                    break;
                case QUAD_STATE:
                    if (strcmp(tokens[0], "Q") == 0 && num_toks == 4) {
                        Q.x = atof(tokens[1]);
                        Q.y = atof(tokens[2]);
                        Q.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "u") == 0 && num_toks == 4) {
                        u.x = atof(tokens[1]);
                        u.y = atof(tokens[2]);
                        u.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "v") == 0 && num_toks == 4) {
                        v.x = atof(tokens[1]);
                        v.y = atof(tokens[2]);
                        v.z = atof(tokens[3]);
                    }
                    break;
                case CAMERA_STATE:
                    if (strcmp(tokens[0], "aspect_ratio") == 0 && num_toks == 3) {
                        aspect_ratio_width = atof(tokens[1]);
                        aspect_ratio_height = atof(tokens[2]);
                        aspect_ratio = aspect_ratio_width / aspect_ratio_height;
                    }
                    else if (strcmp(tokens[0], "width") == 0 && num_toks == 2) {
                        width = atoi(tokens[1]);
                    }
                    break;
                case MATERIAL_STATE:
                    if (strcmp(tokens[0], "name") == 0 && num_toks == 2) {
                        strcpy(mat_name, tokens[1]);
                    }
                    else if (strcmp(tokens[0], "type") == 0 && num_toks == 2) {
                        strcpy(mat_type, tokens[1]);
                    }
                    else if (strcmp(tokens[0], "colour") == 0 && num_toks == 4) {
                        color.x = atof(tokens[1]);
                        color.y = atof(tokens[2]);
                        color.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "fuzz") == 0 && num_toks == 2) {
                        fuzz = atof(tokens[1]);
                    }
                    break;
                case TOPLEVEL_STATE:
                    break;
            }
        }
    }
    fclose(file);

    if (make_camera) {
        Camera cam = camera_make(width, aspect_ratio);
        camera_render(&cam, hittable_world, out_file);
    }
}
