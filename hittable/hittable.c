#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/generic_types.h"
#include "core/vec3.h"
#include "hittable.h"
#include "hit_record.h"
#include "plane.h"
#include "sphere.h"
#include "triangle.h"

void hitrec_set_face_normal(HitRecord *rec, Ray r, Vec3 outward_normal) {
  assert(rec != NULL);
  rec->front_face = vec3_dot(r.direction, outward_normal) < 0;
  rec->normal =
      rec->front_face ? outward_normal : vec3_scale(outward_normal, -1);
}

extern void hittable_destroy(Hittable *self) { self->destroy(self); }

void hittable_print(const Hittable *self) {
  switch (self->type) {
  case HITTABLE_SPHERE:
    sphere_print(self->data);
    break;
  case HITTABLE_PLANE:
    plane_print(self->data);
    break;
  case HITTABLE_TRIANGLE:
    triangle_print(self->data);
    break;
  case HITTABLE_QUAD:
    quad_print(self->data);
    break;
  default:
    assert(false);
  }
}
