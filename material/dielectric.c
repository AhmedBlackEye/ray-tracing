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

typedef struct Dielectric {
  // Refractive index in vacuum or air, or the ratio of the material's
  // refractive index over
  // the refractive index of the enclosing media
  double refraction_index;
} Dielectric;

static double schlick_reflectance_approx(double cosine,
                                         double refraction_index);

static bool dielectric_scatter(const Material *self, Ray ray_in, HitRecord *rec,
                               Color *attenuation, Ray *scattered) {
  assert(self != NULL);
  assert(rec != NULL);

  Dielectric *dielectric = self->data;

  *attenuation = (Color){1.0, 1.0, 1.0};
  double ri = rec->front_face ? (1 / dielectric->refraction_index)
                              : dielectric->refraction_index;
  Vec3 unit_direction = vec3_normalized(ray_in.direction);
  double cos_theta =
      fmin(vec3_dot(vec3_negate(unit_direction), rec->normal), 1.0);
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

  bool cannot_refract = ri * sin_theta > 1.0;
  Vec3 direction;

  if (cannot_refract ||
      schlick_reflectance_approx(cos_theta, ri) > random_double()) {
    direction = vec3_reflect(unit_direction, rec->normal);
  } else {
    direction = vec3_refract(unit_direction, rec->normal, ri);
  }
  *scattered = (Ray){.origin = rec->p, .direction = direction, .time = ray_in.time};
  return true;
}

static void dielectric_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

Material *dielectric_create(double refractive_index) {
  Material *mat = malloc(sizeof(struct Material));
  assert(mat != NULL);

  Dielectric *dielectric = malloc(sizeof(struct Dielectric));
  assert(dielectric != NULL);

  dielectric->refraction_index = refractive_index;

  mat->type = MATERIAL_DIELECTRIC;
  mat->scatter = (ScatterFn)dielectric_scatter;
  mat->destroy = (MaterialDestroyFn)dielectric_destroy;
  mat->emitted = NULL;
  mat->data = dielectric;

  return mat;
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

// Use Schlick's approximation for reflectance.
static double schlick_reflectance_approx(double cosine,
                                         double refraction_index) {
  double r0 = (1 - refraction_index) / (1 + refraction_index);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}