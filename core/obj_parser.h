#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include "core/vec3.h"
#include "hittable/hittable.h"
#include "material/material.h"
#include <stdbool.h>
#include <stdio.h>

// Simple OBJ parsing result
typedef struct ObjParseResult {
  bool success;
  char error_message[256];
  int vertex_count;
  int face_count;
} ObjParseResult;

// Simple OBJ parsing - just load the model as-is
extern ObjParseResult obj_parse_file(const char *filename,
                                     Hittable *mesh_hittable, Vec3 scale,
                                     Vec3 translation, Vec3 rotation);

// Utility functions for parsing lines
bool obj_parse_vertex(const char *line, Vec3 *vertex);
bool obj_parse_face(const char *line, int *v1, int *v2, int *v3);

// Print parsing results
extern void obj_print_statistics(const ObjParseResult *result);

#endif // OBJ_PARSER_H