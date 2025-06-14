#include "core/dyn_array.h"
#include "hittable/hittable.h"
#include "material/material.h"

typedef struct Scene {
  DynArray *objects;
  DynArray *materials;
} Scene;

extern Scene scene_create();
extern void scene_destroy(Scene *self);