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
#include "texture/solid_color.h"

typedef struct Lambertian {
  Texture *tex;
} Lambertian;

static bool lambertian_scatter(const Material *self, Ray ray_in, HitRecord *rec,
                               Color *attenuation, Ray *scattered) {
  assert(self != NULL);
  assert(rec != NULL);

  Lambertian *lamb = self->data;

  Vec3 scatter_direction = vec3_add(rec->normal, vec3_random_unit_vector());
  // Catch degenerate scatter direction
  if (vec3_is_near_zero(scatter_direction))
    scatter_direction = rec->normal;
  *scattered = (Ray){.origin = rec->p, .direction = scatter_direction, .time = ray_in.time};
  *attenuation = lamb->tex->value(lamb->tex, rec->u, rec->v, &rec->p);

  return true;
}

static void lambertian_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

Material *lambertian_create_texture(Texture *tex) {
  Material *mat = malloc(sizeof(struct Material));
  assert(mat != NULL);

  Lambertian *lamb = malloc(sizeof(struct Lambertian));
  assert(lamb != NULL);

  lamb->tex = tex;

  mat->type = MATERIAL_LAMBERTIAN;
  mat->scatter = (ScatterFn)lambertian_scatter;
  mat->destroy = (MaterialDestroyFn)lambertian_destroy;
  mat->emitted = NULL;
  mat->data = lamb;

  return mat;
}

Material *lambertian_create(Color albedo) {
    SolidColor* sol_col = solid_color_create_albedo(&albedo);
    return lambertian_create_texture((Texture*)sol_col);
}

void lambertian_print(const Material *self) {
  if (self == NULL || self->type != MATERIAL_LAMBERTIAN) {
    printf("Lambertian: Invalid or NULL\n");
    return;
  }
  //const Lambertian *lamb = (const Lambertian *)self->data;
  //Color albedo = lamb->albedo;
  //printf("Lambertian { albedo: %.3f %.3f %.3f}\n", albedo.x, albedo.y, albedo.z);
}
