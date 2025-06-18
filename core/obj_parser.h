#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include "core/vec3.h"
#include "hittable/hittable.h"
#include "material/material.h"
#include <stdbool.h>
#include <stdio.h>

// OBJ parsing result structure
typedef struct ObjParseResult {
  bool success;
  char error_message[256];
  int vertex_count;
  int face_count;
} ObjParseResult;

// SINGLE parsing function - always uses transforms
ObjParseResult obj_parse_file(const char *filename, Hittable *mesh_hittable,
                              Vec3 scale, Vec3 translation, Vec3 rotation);

// Utility functions for parsing lines
bool obj_parse_vertex(const char *line, Vec3 *vertex);
bool obj_parse_face(const char *line, int *v1, int *v2, int *v3);

// Print parsing results
void obj_print_statistics(const ObjParseResult *result);

// Helper for no transforms (identity transform)
static inline Vec3 obj_no_scale(void) { return (Vec3){1.0, 1.0, 1.0}; }
static inline Vec3 obj_no_translation(void) { return (Vec3){0.0, 0.0, 0.0}; }
static inline Vec3 obj_no_rotation(void) { return (Vec3){0.0, 0.0, 0.0}; }

#endif // OBJ_PARSER_H