#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/dyn_array.h"
#include "core/vec3.h"
#include "hit_record.h"
#include "triangle.h"
#include "triangle_mesh.h"

/**
 * This creates the triangle but only for mesh, instead of creating hittables,
 * we create just triangles Meaning: We leave the triangle destroy and triangle
 * hit functions to the triangles we have made but we create the Triangle object
 * The Triangle object does not have a destroy ot hit, instead we call these
 * functions to the actual functions we have declared for the triangles to make
 * the 3D object.
 */

typedef struct BoundingBox {
  double max;
  double min;
};

typedef struct Mesh {
  DynArray *Triangle;
  BoundingBox bounds;
} Mesh;

bool mesh_hit

    // Utility function for when creating meshes from objects.
    // BoundingBox compute_mesh_bounds(const TriangleMesh *mesh) {
    //   if (mesh->triangle_count == 0) {
    //     return (BoundingBox){{0, 0, 0}, {0, 0, 0}};
    //   }

    //   Triangle *first_tri = (Triangle *)dynarray_get(mesh->triangles, 0);
    //   BoundingBox bounds = {first_tri->v0, first_tri->v0};

    //   for (int i = 0; i < mesh->triangle_count; i++) {
    //     Triangle *tri = (Triangle *)dynarray_get(mesh->triangles, i);
    //     Vec3 vertices[3] = {tri->v0, tri->v1, tri->v2};

    //     for (int v = 0; v < 3; v++) {
    //       bounds.min.x = MIN(bounds.min.x, vertices[v].x);
    //       bounds.min.y = MIN(bounds.min.y, vertices[v].y);
    //       bounds.min.z = MIN(bounds.min.z, vertices[v].z);

    //       bounds.max.x = MAX(bounds.max.x, vertices[v].x);
    //       bounds.max.y = MAX(bounds.max.y, vertices[v].y);
    //       bounds.max.z = MAX(bounds.max.z, vertices[v].z);
    //     }
    //   }

    //   return bounds;
    // }

    /**
     * What to do:
     * Triangle_mesh currently holds individual hittable triangle objects
     * (meaning they all have their own destry function and hit function which
     * are the same) We want to abstract these functions out so that we only
     * store the Triangle object and not a Triangle Hittable object. When we
     * want to destroy or check if hit, we just call those functions to that
     * triangle.
     */