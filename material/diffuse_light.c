#include "diffuse_light.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    Color emit_color; 
} DiffuseLightData;

static void diffuse_light_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

void dielectric_print(const Material *self) {
  if (self == NULL || self->type != MATERIAL_LAMBERTIAN) {
    printf("Dielectric: Invalid or NULL\n");
    return;
  }
  const Dielectric *dielectric = (const Dielectric *)self->data;
  printf("Dielectric { refractive_index: %.3f}\n",
         dielectric->refraction_index);
}