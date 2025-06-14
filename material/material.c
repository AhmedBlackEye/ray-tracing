#include <assert.h>

#include "material/material.h"
#include "material/lambertian.h"
#include "material/metal.h"

extern void material_destroy(Material *self) { self->destroy(self); }

void material_print(const Material *self) {
  switch (self->type) {
  case MATERIAL_LAMBERTIAN:
    lambertian_print(self);
    break;
  case MATERIAL_METAL:
    metal_print(self);
    break;
  default:
    assert(false);
  }
}
