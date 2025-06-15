#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/interval.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "triangle_raw.h"

static void triangle_destroy(void *self) {
  assert(self != NULL);
  TriangleRaw *tri = (TriangleRaw *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

void triangle_raw_print(const TriangleRaw *tri) {
  if (tri == NULL) {
    printf("Triangle: NULL.\n");
    return;
  }
  printf("Triangle v0:(%.2f,%.2f,%.2f), v1:(%.2f, %.2f, %.2f), "
         "v2:(%.2f,%.2f,%.2f)\n",
         tri->v0.x, tri->v0.y, tri->v0.z, tri->v1.x, tri->v1.y, tri->v1.z,
         tri->v2.x, tri->v2.y, tri->v2.z);
}

TriangleRaw *triangle_raw_create(Vec3 v0, Vec3 v1, Vec3 v2, Material mat) {
  TriangleRaw *triangle = malloc(sizeof(TriangleRaw));
  assert(triangle != NULL);

  triangle->v0 = v0;
  triangle->v1 = v1;
  triangle->v2 = v2;

  triangle->edge1 = vec3_sub(v1, v0); // plane a
  triangle->edge2 = vec3_sub(v2, v0); // plane b
  triangle->material = mat;

  // printf("x: %f , y: %f, z: %f \n", triangle->edge1.x, triangle->edge1.y,
  //  triangle->edge1.z);

  triangle->normal = vec3_normalized(
      vec3_cross(triangle->edge1, triangle->edge2)); // angle of normal line n

  return triangle;
}