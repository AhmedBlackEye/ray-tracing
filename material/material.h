#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>

#include "core/color.h"

typedef struct HitRecord HitRecord;
typedef struct Material Material;
typedef bool (*ScatterFn)(Material *mat, Ray ray_in, HitRecord *rec,
                          Color *attenuation, Ray *scattered);
typedef void (*MaterialDestroyFn)(Material *self);
typedef void (*MaterialPrintFn)(Material *self);

typedef enum { MATERIAL_LAMBERTIAN, MATERIAL_METAL } MaterialType;

typedef struct Material {
  MaterialType type;
  ScatterFn scatter;
  MaterialDestroyFn destroy;
  void *data;
} Material;

#endif // MATERIAL_H