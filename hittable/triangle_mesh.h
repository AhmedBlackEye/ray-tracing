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

extern TriangleMesh *triangle_mesh_create(void);
extern void triangle_mesh_add_triangle(TriangleMesh *mesh, Vec3 v0, Vec3 v1,
                                       Vec3 v2);
extern void triangle_mesh_destroy(TriangleMesh *mesh);
extern void triangle_mesh_print(TriangleMesh *mesh);
// static void update_bounding_box(BoundingBox *box, Vec3 point);
#endif // TRIANGLE_MESH_H
