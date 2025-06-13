#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "hittable.h"
#include <assert.h>

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH

#define MESH_INITIAL_CAPACITY 10

typedef struct BoundingBox {
  Vec3 min;
  Vec3 max;
} BoundingBox;

typedef struct TriangleMesh {
  DynArray *triangles; // Array of Triangle* objects
  BoundingBox bounds;
  int triangle_count;
} TriangleMesh;

#endif // TRIANGLE_MESH_h