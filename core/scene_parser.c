#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/vec3.h"
#include "hittable/plane.h"
#include "hittable/quad.h"
#include "hittable/sphere.h"
#include "hittable/triangle_hittable.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/material.h"
#include "material/metal.h"
#include "scene_parser.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 20
#define WIDTH 400
#define ASPECT_RATIO (16.0 / 9.0)
#define LOOKFROM ((Vec3){13.0, 2.0, 3.0})
#define LOOKAT ((Vec3){0.0, 0.0, -1.0})
#define VUP ((Vec3){0.0, 1.0, 0.0})
#define VFOV 20.0
#define DEFOCUS_ANGLE 0.6
#define FOCUS_DIST 10.0
#define SAMPLES_PER_PIXEL 10
#define MAX_DEPTH 50

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

static Vec3 parse_vec3(char **tokens) {
  return (Vec3){
      .x = atof(tokens[1]), .y = atof(tokens[2]), .z = atof(tokens[3])};
}

static void parse_camera(char *tokens[], int num_toks, Vec3 *lookfrom,
                         Vec3 *lookat, Vec3 *vup, double *vfov,
                         double *defocus_angle, double *focus_dist,
                         int *samples_per_pixel, int *max_depth,
                         double *aspect_ratio, int *width) {
  if (num_toks == 2 && strcmp(tokens[0], "width") == 0) {
    *width = atoi(tokens[1]);
  } else if (num_toks == 3 && strcmp(tokens[0], "aspect_ratio") == 0) {
    double w = atof(tokens[1]);
    double h = atof(tokens[2]);
    *aspect_ratio = w / h;
  } else if (num_toks == 4 && strcmp(tokens[0], "lookfrom") == 0) {
    *lookfrom = parse_vec3(tokens);
  } else if (num_toks == 4 && strcmp(tokens[0], "lookat") == 0) {
    *lookat = parse_vec3(tokens);
  } else if (num_toks == 4 && strcmp(tokens[0], "vup") == 0) {
    *vup = parse_vec3(tokens);
  } else if (num_toks == 2 && strcmp(tokens[0], "vfov") == 0) {
    *vfov = atof(tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "defocus_angle") == 0) {
    *defocus_angle = atof(tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "focus_distance") == 0) {
    *focus_dist = atof(tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "samples_per_pixel") == 0) {
    *samples_per_pixel = atoi(tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "max_depth") == 0) {
    *max_depth = atoi(tokens[1]);
  }
}

static void parse_material(char *tokens[], int num_toks, char *out_name,
                           char *out_type, Vec3 *out_color, double *out_fuzz,
                           double *out_ref_index) {
  if (num_toks == 2 && strcmp(tokens[0], "name") == 0) {
    strcpy(out_name, tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "type") == 0) {
    strcpy(out_type, tokens[1]);
  } else if (num_toks == 4 && strcmp(tokens[0], "color") == 0) {
    *out_color = parse_vec3(tokens);
  } else if (num_toks == 2 && strcmp(tokens[0], "fuzz") == 0) {
    *out_fuzz = atof(tokens[1]);
  } else if (num_toks == 2 && strcmp(tokens[0], "ref_idx") == 0) {
    *out_ref_index = atof(tokens[1]);
  }
}

static void add_material(Scene *scene, DynArray *mat_names, const char *name,
                         const char *type, Vec3 color, double fuzz,
                         double ref_index) {
  Material *mat;
  if (strcmp(type, "lambertian") == 0) {
    mat = lambertian_create(color);
  } else if (strcmp(type, "metal") == 0) {
    mat = metal_create(color, fuzz);
  } else if (strcmp(type, "dielectric") == 0) {
    mat = dielectric_create(ref_index);
  }
  scene_add_material(scene, mat);
  char *name_dup = strdup(name);
  dynarray_push(mat_names, name_dup);
}

static void parse_geometry(ParserState state, char *tokens[], int num_toks,
                           Vec3 *center, double *radius, Vec3 *point,
                           Vec3 *normal, Vec3 *v0, Vec3 *v1, Vec3 *v2, Vec3 *Q,
                           Vec3 *u, Vec3 *v) {
  if (state == SPHERE_STATE) {
    if (num_toks == 4 && strcmp(tokens[0], "center") == 0) {
      *center = parse_vec3(tokens);
    } else if (num_toks == 2 && strcmp(tokens[0], "radius") == 0) {
      *radius = atof(tokens[1]);
    }
  } else if (state == PLANE_STATE) {
    if (num_toks == 4 && strcmp(tokens[0], "point") == 0) {
      *point = parse_vec3(tokens);
    } else if (num_toks == 4 && strcmp(tokens[0], "normal") == 0) {
      *normal = parse_vec3(tokens);
    }
  } else if (state == TRIANGLE_STATE) {
    if (num_toks == 4 && strcmp(tokens[0], "v0") == 0) {
      *v0 = parse_vec3(tokens);
    } else if (num_toks == 4 && strcmp(tokens[0], "v1") == 0) {
      *v1 = parse_vec3(tokens);
    } else if (num_toks == 4 && strcmp(tokens[0], "v2") == 0) {
      *v2 = parse_vec3(tokens);
    }
  } else if (state == QUAD_STATE) {
    if (num_toks == 4 && strcmp(tokens[0], "Q") == 0) {
      *Q = parse_vec3(tokens);
    } else if (num_toks == 4 && strcmp(tokens[0], "u") == 0) {
      *u = parse_vec3(tokens);
    } else if (num_toks == 4 && strcmp(tokens[0], "v") == 0) {
      *v = parse_vec3(tokens);
    }
  }
}

void parse_scene(const char *filename, Scene *scene, Camera *out_cam) {
  FILE *file = fopen(filename, "r");
  assert(file != NULL);

  DynArray *mat_names = dynarray_create(8, NULL, (GDestroyFn)free);

  ParserState state = TOPLEVEL_STATE;

  char line[MAX_LINE_LENGTH];
  char *tokens[MAX_TOKENS];

  Vec3 center;
  double radius;

  Vec3 normal;
  Vec3 point;

  Vec3 lookfrom = LOOKFROM;
  Vec3 lookat = LOOKAT;
  Vec3 vup = VUP;
  double vfov = VFOV;
  double defocus_angle = DEFOCUS_ANGLE;
  double focus_dist = FOCUS_DIST;
  int samples_per_pixel = SAMPLES_PER_PIXEL;
  int max_depth = MAX_DEPTH;
  double aspect_ratio = ASPECT_RATIO;
  int width = WIDTH;

  Material *current_mat;

  char mat_name[32];
  char mat_type[32];
  Vec3 color;
  double fuzz;
  double ref_index;

  Vec3 v0;
  Vec3 v1;
  Vec3 v2;

  Vec3 Q;
  Vec3 u;
  Vec3 v;

  while (fgets(line, MAX_LINE_LENGTH, file)) {
    if (line[0] == '\n') {
      continue;
    }

    if (strchr(line, '}')) {
      switch (state) {
      case MATERIAL_STATE:
        add_material(scene, mat_names, mat_name, mat_type, color, fuzz,
                     ref_index);
        break;
      case SPHERE_STATE:
        scene_add_obj(scene, sphere_create(center, radius, current_mat));
        break;
      case PLANE_STATE:
        scene_add_obj(scene, plane_create(point, normal, current_mat));
        break;
      case TRIANGLE_STATE:
        scene_add_obj(scene, triangle_create(v0, v1, v2, current_mat));
        break;
      case QUAD_STATE:
        scene_add_obj(scene, quad_create(Q, u, v, current_mat));
        break;
      default:
        break;
      }
      state = TOPLEVEL_STATE;
      continue;
    }

    int num_toks = tokenize(line, tokens);
    if (num_toks == 0)
      continue;

    if (state == TOPLEVEL_STATE) {
      if (strcmp(tokens[0], "camera") == 0) {
        state = CAMERA_STATE;
      } else if (strcmp(tokens[0], "material") == 0) {
        state = MATERIAL_STATE;
      } else if (strcmp(tokens[0], "sphere") == 0) {
        state = SPHERE_STATE;
      } else if (strcmp(tokens[0], "plane") == 0) {
        state = PLANE_STATE;
      } else if (strcmp(tokens[0], "triangle") == 0) {
        state = TRIANGLE_STATE;
      } else if (strcmp(tokens[0], "quad") == 0) {
        state = QUAD_STATE;
      }
    } else {
      if (state == CAMERA_STATE) {
        parse_camera(tokens, num_toks, &lookfrom, &lookat, &vup, &vfov,
                     &defocus_angle, &focus_dist, &samples_per_pixel,
                     &max_depth, &aspect_ratio, &width);
      } else if (state == MATERIAL_STATE) {
        parse_material(tokens, num_toks, mat_name, mat_type, &color, &fuzz,
                       &ref_index);
      } else {
        if (num_toks == 2 && strcmp(tokens[0], "material") == 0) {
          size_t num_mats = dynarray_size(mat_names);
          for (size_t i = 0; i < num_mats; i++) {
            char *p = (char *)dynarray_get(mat_names, i);
            if (p != NULL && strcmp(tokens[1], p) == 0) {
              current_mat = (Material *)dynarray_get(scene->materials, i);
              break;
            }
          }
          continue;
        }
        parse_geometry(state, tokens, num_toks, &center, &radius, &point,
                       &normal, &v0, &v1, &v2, &Q, &u, &v);
      }
    }
  }
  fclose(file);

  *out_cam =
      camera_make(width, aspect_ratio, lookfrom, lookat, vup, vfov,
                  defocus_angle, focus_dist, samples_per_pixel, max_depth);

  dynarray_destroy(mat_names);
}