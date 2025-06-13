#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdbool.h>

typedef struct HitRecord HitRecord; 
typedef bool (*ScatterFn)(Ray ray_in, HitRecord *rec, Color attenuation,
                          Ray scattered);
typedef void (*MaterialDestroyFn)(Material *self);
typedef void (*MaterialPrintFn)(Material *self);

typedef enum { MATERIAL_LAMBERTIAN } MaterialType;

typedef struct Material {
  MaterialType type;
  ScatterFn scatter;
  MaterialDestroyFn destroy;
  void *data;
} Material;

#endif // MATERIAL_H