#include "obj_parser.h"
#include "core/vec3.h"
#include "hittable/triangle_mesh.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
#define INITIAL_VERTEX_CAPACITY 1000

// ===== TRANSFORM FUNCTIONS =====

static Vec3 rotate_x(Vec3 v, double angle) {
  if (angle == 0.0)
    return v; // Skip if no rotation
  double cos_a = cos(angle);
  double sin_a = sin(angle);
  return (Vec3){v.x, v.y * cos_a - v.z * sin_a, v.y * sin_a + v.z * cos_a};
}

static Vec3 rotate_y(Vec3 v, double angle) {
  if (angle == 0.0)
    return v; // Skip if no rotation
  double cos_a = cos(angle);
  double sin_a = sin(angle);
  return (Vec3){v.x * cos_a + v.z * sin_a, v.y, -v.x * sin_a + v.z * cos_a};
}

static Vec3 rotate_z(Vec3 v, double angle) {
  if (angle == 0.0)
    return v; // Skip if no rotation
  double cos_a = cos(angle);
  double sin_a = sin(angle);
  return (Vec3){v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a, v.z};
}

static void apply_transforms(Vec3 *vertex, Vec3 scale, Vec3 position,
                             Vec3 rotation) {
  // 1. Scale first
  vertex->x *= scale.x;
  vertex->y *= scale.y;
  vertex->z *= scale.z;

  // 2. Rotate (X, Y, Z order)
  *vertex = rotate_x(*vertex, rotation.x);
  *vertex = rotate_y(*vertex, rotation.y);
  *vertex = rotate_z(*vertex, rotation.z);

  // 3. Set absolute position (not translate)
  *vertex = vec3_add(*vertex, position);
}

// ===== PARSING FUNCTIONS =====

bool obj_parse_vertex(const char *line, Vec3 *vertex) {
  if (line[0] != 'v' || line[1] != ' ') {
    return false;
  }

  int result =
      sscanf(line + 2, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);
  return result == 3;
}

// Updated function signature to handle both triangles and quads
bool obj_parse_face(const char *line, int *v1, int *v2, int *v3, int *v4,
                    bool *is_quad) {
  if (line[0] != 'f' || line[1] != ' ') {
    return false;
  }

  int dummy;
  *is_quad = false;

  // Try quad formats first
  int result = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", v1,
                      &dummy, &dummy, v2, &dummy, &dummy, v3, &dummy, &dummy,
                      v4, &dummy, &dummy);
  if (result == 12) {
    *is_quad = true;
    return true;
  }

  result = sscanf(line + 2, "%d//%d %d//%d %d//%d %d//%d", v1, &dummy, v2,
                  &dummy, v3, &dummy, v4, &dummy);
  if (result == 8) {
    *is_quad = true;
    return true;
  }

  result = sscanf(line + 2, "%d/%d %d/%d %d/%d %d/%d", v1, &dummy, v2, &dummy,
                  v3, &dummy, v4, &dummy);
  if (result == 8) {
    *is_quad = true;
    return true;
  }

  // Try simple quad format
  result = sscanf(line + 2, "%d %d %d %d", v1, v2, v3, v4);
  if (result == 4) {
    *is_quad = true;
    return true;
  }

  // Try triangle formats
  result = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", v1, &dummy, &dummy,
                  v2, &dummy, &dummy, v3, &dummy, &dummy);
  if (result == 9) {
    return true;
  }

  result = sscanf(line + 2, "%d//%d %d//%d %d//%d", v1, &dummy, v2, &dummy, v3,
                  &dummy);
  if (result == 6) {
    return true;
  }

  result =
      sscanf(line + 2, "%d/%d %d/%d %d/%d", v1, &dummy, v2, &dummy, v3, &dummy);
  if (result == 6) {
    return true;
  }

  result = sscanf(line + 2, "%d %d %d", v1, v2, v3);
  if (result == 3) {
    return true;
  }

  return false;
}

ObjParseResult obj_parse_file_to_hittables(const char *filename,
                                           MeshLoader *loader,
                                           Hittable *hittable_list, Vec3 scale,
                                           Vec3 position, Vec3 rotation) {
  ObjParseResult result = {0};

  // Validate input
  if (!filename || !loader || !hittable_list) {
    result.success = false;
    strcpy(result.error_message, "Invalid input parameters");
    return result;
  }

  if (hittable_list->type != HITTABLE_LIST) {
    result.success = false;
    strcpy(result.error_message, "Target must be a hittable list");
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

  // Check if we have any transforms to apply
  bool has_transforms =
      (scale.x != 1.0 || scale.y != 1.0 || scale.z != 1.0 ||
       position.x != 0.0 || position.y != 0.0 || position.z != 0.0 ||
       rotation.x != 0.0 || rotation.y != 0.0 || rotation.z != 0.0);

  printf("Parsing OBJ file: %s", filename);
  if (has_transforms) {
    printf(" (with transforms)");
  }
  printf("\n");

  // Create vertex storage using DynArray
  DynArray *vertices = dynarray_create(100, NULL, (GDestroyFn)free);
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
      Vec3 *vertex = malloc(sizeof(Vec3));
      assert(vertex != NULL);

      if (obj_parse_vertex(line, vertex)) {
        // Apply transforms
        apply_transforms(vertex, scale, position, rotation);

        // Add to dynamic array
        dynarray_push(vertices, vertex);
        result.vertex_count++;

        // Debug output for first few vertices
        static int debug_count = 0;
        if (debug_count < 3) {
          printf("  Vertex %d: (%.3f, %.3f, %.3f)\n", debug_count + 1,
                 vertex->x, vertex->y, vertex->z);
          debug_count++;
        }
      } else {
        free(vertex);
        result.success = false;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Invalid vertex format at line %d", line_number);
        dynarray_destroy(vertices);
        fclose(file);
        return result;
      }
    }
    // Parse face lines (f v1 v2 v3 or f v1 v2 v3 v4)
    else if (line[0] == 'f' && line[1] == ' ') {
      int v1, v2, v3, v4;
      bool is_quad;

      if (obj_parse_face(line, &v1, &v2, &v3, &v4, &is_quad)) {
        // OBJ files use 1-based indexing, convert to 0-based
        v1--;
        v2--;
        v3--;
        if (is_quad)
          v4--;

        // Validate indices
        int vertex_count = dynarray_size(vertices);
        if (v1 < 0 || v1 >= vertex_count || v2 < 0 || v2 >= vertex_count ||
            v3 < 0 || v3 >= vertex_count ||
            (is_quad && (v4 < 0 || v4 >= vertex_count))) {
          result.success = false;
          snprintf(result.error_message, sizeof(result.error_message),
                   "Invalid vertex index at line %d", line_number);
          dynarray_destroy(vertices);
          fclose(file);
          return result;
        }

        // Get vertices
        Vec3 vert1 = *(Vec3 *)dynarray_get(vertices, v1);
        Vec3 vert2 = *(Vec3 *)dynarray_get(vertices, v2);
        Vec3 vert3 = *(Vec3 *)dynarray_get(vertices, v3);

        if (is_quad) {
          Vec3 vert4 = *(Vec3 *)dynarray_get(vertices, v4);

          // Split quad into two triangles: (v1,v2,v3) and (v1,v3,v4)
          mesh_loader_add_triangle(loader, hittable_list, vert1, vert2, vert3);
          mesh_loader_add_triangle(loader, hittable_list, vert1, vert3, vert4);
          result.face_count += 2;
        } else {
          // Single triangle
          mesh_loader_add_triangle(loader, hittable_list, vert1, vert2, vert3);
          result.face_count++;
        }
      } else {
        result.success = false;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Invalid face format at line %d", line_number);
        dynarray_destroy(vertices);
        fclose(file);
        return result;
      }
    }
    // Ignore other line types (vt, vn, etc.)
  }

  // Cleanup
  dynarray_destroy(vertices);
  fclose(file);

  if (result.vertex_count == 0) {
    result.success = false;
    strcpy(result.error_message, "No vertices found in OBJ file");
    printf("ERROR: %s\n", result.error_message);
    return result;
  }

  if (result.face_count == 0) {
    result.success = false;
    strcpy(result.error_message, "No faces found in OBJ file");
    printf("ERROR: %s\n", result.error_message);
    return result;
  }

  result.success = true;
  printf("=== Successfully loaded OBJ file! ===\n");
  printf("  Vertices: %d\n", result.vertex_count);
  printf("  Faces: %d\n", result.face_count);
  if (has_transforms) {
    printf("  Applied transforms: scale(%.1f,%.1f,%.1f) pos(%.1f,%.1f,%.1f) "
           "rot(%.0f°,%.0f°,%.0f°)\n",
           scale.x, scale.y, scale.z, position.x, position.y, position.z,
           rotation.x * 180.0 / M_PI, rotation.y * 180.0 / M_PI,
           rotation.z * 180.0 / M_PI);
  }
  printf("=====================================\n");

  return result;
}