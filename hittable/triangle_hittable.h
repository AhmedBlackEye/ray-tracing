#ifndef TRIANGLE_HITTABLE_H
#define TRIANGLE_HITTABLE_H

#include "hittable.h"
#include "material/material.h"
#include "triangle_raw.h"

typedef struct TriangleHittable {
  TriangleRaw tri;
} TriangleHittable;

#endif