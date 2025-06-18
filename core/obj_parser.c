#include "obj_parser.h"
#include "core/vec3.h"
#include "hittable/triangle_mesh.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
#define INITIAL_VERTEX_CAPACITY 1000

// Simple dynamic array for storing vertices during parsing
typedef struct VertexArray {
  Vec3 *vertices;
  int count;
  int capacity;
} VertexArray;

static VertexArray *vertex_array_create() {
  VertexArray *arr = malloc(sizeof(VertexArray));
  assert(arr != NULL);

  arr->vertices = malloc(sizeof(Vec3) * INITIAL_VERTEX_CAPACITY);
  assert(arr->vertices != NULL);

  arr->count = 0;
  arr->capacity = INITIAL_VERTEX_CAPACITY;
  return arr;
}

static void vertex_array_destroy(VertexArray *arr) {
  if (arr) {
    free(arr->vertices);
    free(arr);
  }
}

static void vertex_array_push(VertexArray *arr, Vec3 vertex) {
  if (arr->count >= arr->capacity) {
    arr->capacity *= 2;
    arr->vertices = realloc(arr->vertices, sizeof(Vec3) * arr->capacity);
    assert(arr->vertices != NULL);
  }
  arr->vertices[arr->count++] = vertex;
}

static Vec3 vertex_array_get(VertexArray *arr, int index) {
  assert(index >= 0 && index < arr->count);
  return arr->vertices[index];
}

bool obj_parse_vertex(const char *line, Vec3 *vertex) {
  // Check if line starts with "v "
  if (line[0] != 'v' || line[1] != ' ') {
    return false;
  }

  // Parse three floating point numbers
  int result =
      sscanf(line + 2, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);
  return result == 3;
}

bool obj_parse_face(const char *line, int *v1, int *v2, int *v3) {
  // Check if line starts with "f "
  if (line[0] != 'f' || line[1] != ' ') {
    return false;
  }

  // Handle different face formats:
  // f v1 v2 v3 (just vertex indices)
  // f v1/vt1 v2/vt2 v3/vt3 (vertex/texture - ignore texture)
  // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 (vertex/texture/normal - ignore texture
  // and normal) f v1//vn1 v2//vn2 v3//vn3 (vertex//normal - ignore normal)

  int dummy; // For texture/normal indices we don't use

  // Try vertex/texture/normal format first
  int result = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", v1, &dummy,
                      &dummy, v2, &dummy, &dummy, v3, &dummy, &dummy);
  if (result == 9)
    return true;

  // Try vertex//normal format
  result = sscanf(line + 2, "%d//%d %d//%d %d//%d", v1, &dummy, v2, &dummy, v3,
                  &dummy);
  if (result == 6)
    return true;

  // Try vertex/texture format
  result =
      sscanf(line + 2, "%d/%d %d/%d %d/%d", v1, &dummy, v2, &dummy, v3, &dummy);
  if (result == 6)
    return true;

  // Try simple vertex-only format
  result = sscanf(line + 2, "%d %d %d", v1, v2, v3);
  return result == 3;
}

ObjParseResult obj_parse_file(const char *filename, Hittable *mesh_hittable) {
  ObjParseResult result = {0};

  // Validate input
  if (!filename || !mesh_hittable) {
    result.success = false;
    strcpy(result.error_message, "Invalid input parameters");
    return result;
  }

  if (mesh_hittable->type != HITTABLE_TRIANGLE_MESH) {
    result.success = false;
    strcpy(result.error_message, "Hittable must be a triangle mesh");
    return result;
  }

  // Try to open the file
  FILE *file = fopen(filename, "r");
  if (!file) {
    result.success = false;
    snprintf(result.error_message, sizeof(result.error_message),
             "Could not open file: %s", filename);
    return result;
  }

  printf("Parsing OBJ file: %s\n", filename);

  VertexArray *vertices = vertex_array_create();
  char line[MAX_LINE_LENGTH];
  int line_number = 0;

  // Read file line by line
  while (fgets(line, sizeof(line), file)) {
    line_number++;

    // Remove newline character
    line[strcspn(line, "\n")] = '\0';

    // Skip empty lines and comments
    if (line[0] == '\0' || line[0] == '#') {
      continue;
    }

    // Parse vertex lines (v x y z)
    if (line[0] == 'v' && line[1] == ' ') {
      Vec3 vertex;
      if (obj_parse_vertex(line, &vertex)) {
        vertex_array_push(vertices, vertex);
        result.vertex_count++;
      } else {
        result.success = false;
        printf("Invalid vertex format at line %d: %s", line_number, line);
        fclose(file);
        vertex_array_destroy(vertices);
        return result;
      }
    }
    // Parse face lines (f v1 v2 v3)
    else if (line[0] == 'f' && line[1] == ' ') {
      int v1, v2, v3;
      if (obj_parse_face(line, &v1, &v2, &v3)) {

        // Check if indices are valid
        if (v1 < 0 || v1 >= vertices->count || v2 < 0 ||
            v2 >= vertices->count || v3 < 0 || v3 >= vertices->count) {
          result.success = false;
          printf("Invalid vertex index at line %d: %s", line_number, line);
          fclose(file);
          vertex_array_destroy(vertices);
          return result;
        }

        // Get the three vertices and create a triangle
        Vec3 vert1 = vertex_array_get(vertices, v1);
        Vec3 vert2 = vertex_array_get(vertices, v2);
        Vec3 vert3 = vertex_array_get(vertices, v3);

        // Add triangle to the mesh
        mesh_add_triangle(mesh_hittable, vert1, vert2, vert3);
        result.face_count++;
      } else {
        result.success = false;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Invalid face format at line %d: %s", line_number, line);
        fclose(file);
        vertex_array_destroy(vertices);
        return result;
      }
    }
    // Ignore other line types (vn for normals, vt for textures, o for objects,
    // etc.)
  }

  result.success = true;
  printf("Successfully loaded OBJ file!\n");
  printf("  Vertices: %d\n", result.vertex_count);
  printf("  Faces: %d\n", result.face_count);
}
void obj_print_statistics(const ObjParseResult *result) {
  if (!result)
    return;

  if (result->success) {
    printf("OBJ Parse Result:\n");
    printf("  ✓ Successfully loaded\n");
    printf("  ✓ Vertices: %d\n", result->vertex_count);
    printf("  ✓ Triangles: %d\n", result->face_count);
  } else {
    printf("OBJ Parse Result:\n");
    printf("  ✗ Failed to load\n");
    printf("  ✗ Error: %s\n", result->error_message);
  }
}
