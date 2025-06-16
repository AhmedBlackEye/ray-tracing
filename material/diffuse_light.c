#include "diffuse_light.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct DiffuseLight {
    Color emit_color; 
} DiffuseLight;

static bool diffuse_light_scatter(Material *mat, Ray ray_in, HitRecord *rec,
                                  Color *attenuation, Ray *scattered) {
    return false;
}

static void diffuse_light_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

void diffuse_light_print(const Material *self) {
  if (self == NULL || self->type != MATERIAL_DIFFUSE_LIGHT) {
    printf("Diffuse Light: Invalid or NULL\n");
    return;
  }
  const DiffuseLight *diff_l = (const DiffuseLight *)self->data;
  Color emit_color = diff_l->emit_color;
  printf("Diffuse Light { emit colour: %.3f %.3f %.3f}\n", emit_color.x, emit_color.y, emit_color.z);
}

Color diffuse_light_emitted(Material *self, double u, double v, const Vec3 *p) {
    if (!self || self->type != MATERIAL_DIFFUSE_LIGHT) {
        return vec3_zero();
    }
    
    DiffuseLight *data = (DiffuseLight*)self->data;
    return data->emit_color;
}

Material *diffuse_light_create(Color emit_color) {
  Material *mat = malloc(sizeof(struct Material));
  assert(mat != NULL);

  DiffuseLight *diff_l = malloc(sizeof(struct DiffuseLight));
  assert(diff_l != NULL);

  diff_l->emit_color = emit_color;

  mat->type = MATERIAL_DIFFUSE_LIGHT;
  mat->scatter = (ScatterFn)diffuse_light_scatter;
  mat->destroy = (MaterialDestroyFn)diffuse_light_destroy;
  mat->emitted = diffuse_light_emitted;
  mat->data = diff_l;

  return mat;
}
