#include <assert.h>

#include "material/material.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/dielectric.h"
#include "material/diffuse_light.h"

extern void material_destroy(Material *self) { self->destroy(self); }

void material_print(const Material *self) {
  switch (self->type) {
  case MATERIAL_LAMBERTIAN:
    lambertian_print(self);
    break;
  case MATERIAL_METAL:
    metal_print(self);
    break;
  case MATERIAL_DIELECTRIC:
    dielectric_print(self);
    break;
  case MATERIAL_DIFFUSE_LIGHT:
    diffuse_light_print(self);
    break;
  default:
    assert(false);
  }
}

Color material_emitted(Material *mat, double u, double v, const Vec3 *p) {
    if (!mat || !mat->emitted) {
        return vec3_zero(); 
    }
    return mat->emitted(mat, u, v, p);
}