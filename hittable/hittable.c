#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh_node.h"
#include "core/generic_types.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "hittable.h"
#include "hittable_list.h"
#include "plane.h"
#include "quad.h"
#include "rotate_y.h"
#include "sphere.h"
#include "translate.h"
#include "triangle_hittable.h"
#include "triangle_mesh.h"

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
  case HITTABLE_LIST:
    hittablelist_print(self);
    break;
  case HITTABLE_BVHNODE:
    bvhnode_print(self);
    break;
  case HITTABLE_TRANSLATE:
    translate_print(self);
    break;
  case HITTABLE_ROTATE_Y:
    rotate_y_print(self);
    break;
  default:
    printf("Unknown hittable type: %d\n", self->type);
    break;
  }
}
