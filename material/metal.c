#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/color.h"
#include "core/interval.h"
#include "core/ray.h"
#include "hittable/hit_record.h"
#include "material.h"

typedef struct Metal {
  Color albedo;
  double fuzz;
} Metal;

static bool metal_scatter(const Material *self, Ray ray_in, HitRecord *rec,
                          Color *attenuation, Ray *scattered) {
  assert(self != NULL);
  assert(rec != NULL);

  Metal *metal = self->data;

  Vec3 reflected = vec3_reflect(ray_in.direction, rec->normal);
  reflected = vec3_add(vec3_normalized(reflected),
                       vec3_scale(vec3_random_unit_vector(), metal->fuzz));
  *scattered = (Ray){.origin = rec->p, .direction = reflected};
  *attenuation = metal->albedo;

  return true;
}

static void metal_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

Material *metal_create(Color albedo, double fuzz) {
  Material *mat = malloc(sizeof(struct Material));
  assert(mat != NULL);

  Metal *metal = malloc(sizeof(struct Metal));
  assert(metal != NULL);

  metal->albedo = albedo;
  metal->fuzz = fuzz;

  mat->type = MATERIAL_METAL;
  mat->scatter = (ScatterFn)metal_scatter;
  mat->destroy = (MaterialDestroyFn)metal_destroy;
  mat->emitted = NULL;
  mat->data = metal;

  return mat;
}

void metal_print(const Material *self) {
  if (self == NULL || self->type != MATERIAL_METAL) {
    printf("Metal: Invalid or NULL\n");
    return;
  }
  const Metal *metal = (const Metal *)self->data;
  Color albedo = metal->albedo;
  printf("Metal { albedo: %.3f %.3f %.3f }\n", albedo.x, albedo.y, albedo.z);
}
