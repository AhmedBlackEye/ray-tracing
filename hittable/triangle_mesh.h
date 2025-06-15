#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "core/dyn_array.h"
#include "core/vec3.h"

#define INIT_CAP 10

typedef struct BoundingBox {
  Vec3 max;
  Vec3 min;
} BoundingBox;

typedef struct TriangleMesh {
  DynArray *raw_triangles;
  BoundingBox bounds;
} TriangleMesh;

#endif // TRIANGLE_MESH_H
