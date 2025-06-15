#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/generic_types.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "plane.h"
#include "quad.h"
#include "sphere.h"
#include "triangle_hittable.h"

extern void hittable_destroy(Hittable *self) { self->destroy(self); }

void hittable_print(const Hittable *self) {
  switch (self->type) {
  case HITTABLE_SPHERE:
    sphere_print(self);
    break;
  case HITTABLE_PLANE:
    plane_print(self);
    break;
  case HITTABLE_TRIANGLE:
    triangle_hittable_print(self);
    break;
  case HITTABLE_QUAD:
    quad_print(self);
    break;
  default:
    assert(false);
  }
}
