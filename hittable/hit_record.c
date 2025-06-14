#include "assert.h"

#include "hit_record.h"
#include "core/vec3.h"
#include "core/ray.h"

void hitrec_set_face_normal(HitRecord *rec, Ray r, Vec3 outward_normal) {
  assert(rec != NULL);
  rec->front_face = vec3_dot(r.direction, outward_normal) < 0;
  rec->normal =
      rec->front_face ? outward_normal : vec3_scale(outward_normal, -1);
}