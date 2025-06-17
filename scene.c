#include "scene.h"
#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "material/material.h"
#include "texture/texture.h"

Scene scene_create() {
  Scene scene;
  scene.objects = hittablelist_empty();
  scene.materials = dynarray_create(2, (GPrintFn)material_print,
                                    (GDestroyFn)material_destroy);
  scene.textures = dynarray_create(2, NULL,
                                    (GDestroyFn)texture_destroy);
  return scene;
}

void scene_destroy(Scene *self) {
  self->objects->destroy(self->objects);
  dynarray_destroy(self->materials);
  dynarray_destroy(self->textures);
}

Hittable *scene_add_obj(Scene *self, Hittable *obj) {
  hittablelist_add(self->objects, obj);
  return obj;
}

Material *scene_add_material(Scene *self, Material *mat) {
  dynarray_push(self->materials, mat);
  return mat;
}

Texture *scene_add_texture(Scene *self, Texture *tex) {
    dynarray_push(self->textures, tex);
    return tex;
}