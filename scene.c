#include "scene.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "hittable/hittable.h"
#include "material/material.h"

Scene scene_create() {
  Scene scene;
  scene.objects = dynarray_create(8, (GPrintFn)hittable_print,
                                  (GDestroyFn)hittable_destroy);
  scene.materials = dynarray_create(2, (GPrintFn)material_print,
                                    (GDestroyFn)material_destroy);
  return scene;
}

void scene_destroy(Scene *self) {
  dynarray_destroy(self->objects);
  dynarray_destroy(self->materials);
}

Hittable *scene_add_obj(Scene *self, Hittable *obj) {
  dynarray_push(self->objects, obj);
  return obj;
}

Material *scene_add_material(Scene *self, Material *mat) {
  dynarray_push(self->materials, mat);
  return mat;
}