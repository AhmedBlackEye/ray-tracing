#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>

#include "core/color.h"
#include "core/ray.h"
#include "hittable/hit_record.h"

typedef struct HitRecord HitRecord;
typedef struct Material Material;
typedef bool (*ScatterFn)(Material *mat, Ray ray_in, HitRecord *rec,
                          Color *attenuation, Ray *scattered);
typedef void (*MaterialDestroyFn)(Material *self);
typedef void (*MaterialPrintFn)(Material *self);
typedef Color (*MaterialEmittedFn)(Material *self, double u, double v, const Vec3 *p); 

typedef enum { MATERIAL_LAMBERTIAN, MATERIAL_METAL, MATERIAL_DIELECTRIC, MATERIAL_DIFFUSE_LIGHT } MaterialType;

typedef struct Material {
  MaterialType type;
  ScatterFn scatter;
  MaterialDestroyFn destroy;
  MaterialEmittedFn emitted;
  void *data;
} Material;

extern void material_destroy(Material *self);
extern void material_print(const Material *self);
extern Color material_emitted(Material *mat, double u, double v, const Vec3 *p);

#endif // MATERIAL_Hss