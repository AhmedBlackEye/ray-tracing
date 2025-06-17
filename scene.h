#ifndef SCENE_H
#define SCENE_H

#include "core/dyn_array.h"
#include "hittable/hittable.h"
#include "material/material.h"
#include "texture/texture.h"

typedef struct Scene {
  Hittable *objects;
  DynArray *materials;
  DynArray *textures;
} Scene;

extern Scene scene_create(void);
extern void scene_destroy(Scene *self);

extern Hittable *scene_add_obj(Scene *self, Hittable *obj);
extern Material *scene_add_material(Scene *self, Material *mat);
extern Texture *scene_add_texture(Scene *self, Texture *tex);

#endif