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

static void apply_transforms(Vec3 *vertex, Vec3 scale, Vec3 translation,
                             Vec3 rotation) {
  // 1. Scale first
  vertex->x *= scale.x;
  vertex->y *= scale.y;
  vertex->z *= scale.z;

  // 2. Rotate (X, Y, Z order)
  *vertex = rotate_x(*vertex, rotation.x);
  *vertex = rotate_y(*vertex, rotation.y);
  *vertex = rotate_z(*vertex, rotation.z);

  // 3. Translate last
  *vertex = vec3_add(*vertex, translation);
}

// ===== PARSING FUNCTION =====

bool obj_parse_vertex(const char *line, Vec3 *vertex) {
  if (line[0] != 'v' || line[1] != ' ') {
    return false;
  }

  int result =
      sscanf(line + 2, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);
  return result == 3;
}

bool obj_parse_face(const char *line, int *v1, int *v2, int *v3) {
  if (line[0] != 'f' || line[1] != ' ') {
    return false;
  }

  int dummy;

  // Try different face formats
  int result = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", v1, &dummy,
                      &dummy, v2, &dummy, &dummy, v3, &dummy, &dummy);
  if (result == 9)
    return true;

  result = sscanf(line + 2, "%d//%d %d//%d %d//%d", v1, &dummy, v2, &dummy, v3,
                  &dummy);
  if (result == 6)
    return true;

  result =
      sscanf(line + 2, "%d/%d %d/%d %d/%d", v1, &dummy, v2, &dummy, v3, &dummy);
  if (result == 6)
    return true;

  result = sscanf(line + 2, "%d %d %d", v1, v2, v3);
  if (result == 3) {
    // Debug output for first few faces
    static int face_debug_count = 0;
    if (face_debug_count < 3) {
      printf("  Parsed face %d: %d %d %d\n", face_debug_count, *v1, *v2, *v3);
      face_debug_count++;
    }
    return true;
  }
  return false;
}

ObjParseResult obj_parse_file_to_hittables(const char *filename,
                                           MeshLoader *loader,
                                           Hittable *hittable_list, Vec3 scale,
                                           Vec3 translation, Vec3 rotation) {
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
       translation.x != 0.0 || translation.y != 0.0 || translation.z != 0.0 ||
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
        apply_transforms(vertex, scale, translation, rotation);

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
    // Parse face lines (f v1 v2 v3)
    else if (line[0] == 'f' && line[1] == ' ') {
      int v1, v2, v3;
      if (obj_parse_face(line, &v1, &v2, &v3)) {
        // OBJ files use 1-based indexing, convert to 0-based
        v1--;
        v2--;
        v3--;

        // Check if indices are valid
        int vertex_count = dynarray_size(vertices);
        if (v1 < 0 || v1 >= vertex_count || v2 < 0 || v2 >= vertex_count ||
            v3 < 0 || v3 >= vertex_count) {
          result.success = false;
          snprintf(result.error_message, sizeof(result.error_message),
                   "Invalid vertex index at line %d", line_number);
          dynarray_destroy(vertices);
          fclose(file);
          return result;
        }

        // Get the three vertices from DynArray
        Vec3 *vert1_ptr = (Vec3 *)dynarray_get(vertices, v1);
        Vec3 *vert2_ptr = (Vec3 *)dynarray_get(vertices, v2);
        Vec3 *vert3_ptr = (Vec3 *)dynarray_get(vertices, v3);

        Vec3 vert1 = *vert1_ptr;
        Vec3 vert2 = *vert2_ptr;
        Vec3 vert3 = *vert3_ptr;

        // Debug output for first few triangles
        if (result.face_count < 3) {
          printf("  Triangle %d:\n", result.face_count);
          printf("    v1 (idx %d): (%.3f, %.3f, %.3f)\n", v1, vert1.x, vert1.y,
                 vert1.z);
          printf("    v2 (idx %d): (%.3f, %.3f, %.3f)\n", v2, vert2.x, vert2.y,
                 vert2.z);
          printf("    v3 (idx %d): (%.3f, %.3f, %.3f)\n", v3, vert3.x, vert3.y,
                 vert3.z);
        }

        // *** KEY PART: Add triangle as individual hittable ***
        mesh_loader_add_triangle(loader, hittable_list, vert1, vert2, vert3);
        result.face_count++;
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
  printf("  Faces: %d (each now an individual hittable)\n", result.face_count);
  if (has_transforms) {
    printf("  Applied transforms: scale(%.1f,%.1f,%.1f) pos(%.1f,%.1f,%.1f) "
           "rot(%.0f°,%.0f°,%.0f°)\n",
           scale.x, scale.y, scale.z, translation.x, translation.y,
           translation.z, rotation.x * 180.0 / M_PI, rotation.y * 180.0 / M_PI,
           rotation.z * 180.0 / M_PI);
  }
  printf("=====================================\n");

  return result;
}

// ===== OLD PARSING FUNCTION =====

// ObjParseResult obj_parse_file(const char *filename, Hittable *mesh_hittable,
//                               Vec3 scale, Vec3 translation, Vec3 rotation) {
//   ObjParseResult result = {0};
//   // Validate input
//   if (!filename || !mesh_hittable) {
//     result.success = false;
//     strcpy(result.error_message, "Invalid input parameters");
//     return result;
//   }

//   if (mesh_hittable->type != HITTABLE_TRIANGLE_MESH) {
//     result.success = false;
//     strcpy(result.error_message, "Hittable must be a triangle mesh");
//     return result;
//   }

//   // Try to open the file
//   FILE *file = fopen(filename, "r");
//   if (!file) {
//     result.success = false;
//     snprintf(result.error_message, sizeof(result.error_message),
//              "Could not open file: %s", filename);
//     return result;
//   }

//   // Check if we have any transforms to apply
//   bool has_transforms =
//       (scale.x != 1.0 || scale.y != 1.0 || scale.z != 1.0 ||
//        translation.x != 0.0 || translation.y != 0.0 || translation.z != 0.0
//        || rotation.x != 0.0 || rotation.y != 0.0 || rotation.z != 0.0);

//   printf("Parsing OBJ file: %s", filename);
//   if (has_transforms) {
//     printf(" (with transforms)");
//   }
//   printf("\n");

//   VertexArray *vertices = vertex_array_create();
//   char line[MAX_LINE_LENGTH];
//   int line_number = 0;

//   // Read file line by line
//   while (fgets(line, sizeof(line), file)) {
//     line_number++;

//     // Remove newline character
//     line[strcspn(line, "\n")] = '\0';

//     // Skip empty lines and comments
//     if (line[0] == '\0' || line[0] == '#') {
//       continue;
//     }

//     // Parse vertex lines (v x y z)
//     if (line[0] == 'v' && line[1] == ' ') {
//       Vec3 vertex;
//       if (obj_parse_vertex(line, &vertex)) {
//         // Apply transforms (does nothing if all are identity)
//         apply_transforms(&vertex, scale, translation, rotation);
//         vertex_array_push(vertices, vertex);
//         result.vertex_count++;
//       } else {
//         result.success = false;
//         printf("Invalid vertex format at line %d.", line_number);
//         fclose(file);
//         vertex_array_destroy(vertices);
//         return result;
//       }
//     }
//     // Parse face lines (f v1 v2 v3)
//     else if (line[0] == 'f' && line[1] == ' ') {
//       int v1, v2, v3;
//       if (obj_parse_face(line, &v1, &v2, &v3)) {
//         // OBJ files use 1-based indexing, convert to 0-based
//         v1--;
//         v2--;
//         v3--;

//         // Check if indices are valid
//         if (v1 < 0 || v1 >= vertices->count || v2 < 0 ||
//             v2 >= vertices->count || v3 < 0 || v3 >= vertices->count) {
//           result.success = false;
//           printf("Invalid vertex index at line %d", line_number);
//           fclose(file);
//           vertex_array_destroy(vertices);
//           return result;
//         }

//         // Get the three vertices and create a triangle
//         Vec3 vert1 = vertex_array_get(vertices, v1);
//         Vec3 vert2 = vertex_array_get(vertices, v2);
//         Vec3 vert3 = vertex_array_get(vertices, v3);
//         // Debug output for first few triangles
//         if (result.face_count < 3) {
//           printf("  Triangle %d:\n", result.face_count);
//           printf("    v1 (idx %d): (%.3f, %.3f, %.3f)\n", v1, vert1.x,
//           vert1.y,
//                  vert1.z);
//           printf("    v2 (idx %d): (%.3f, %.3f, %.3f)\n", v2, vert2.x,
//           vert2.y,
//                  vert2.z);
//           printf("    v3 (idx %d): (%.3f, %.3f, %.3f)\n", v3, vert3.x,
//           vert3.y,
//                  vert3.z);
//         }

//         // Add triangle to the mesh
//         mesh_add_triangle(mesh_hittable, vert1, vert2, vert3);
//         result.face_count++;
//       } else {
//         result.success = false;
//         printf("Invalid face format at line %d.", line_number);
//         fclose(file);
//         vertex_array_destroy(vertices);
//         return result;
//       }
//     }
//     // Ignore other line types
//     // vertex_array_print(vertices);
//   }

//   fclose(file);
//   vertex_array_destroy(vertices);

//   if (result.vertex_count == 0) {
//     result.success = false;
//     strcpy(result.error_message, "No vertices found in OBJ file");
//     printf("ERROR: %s\n", result.error_message);
//     return result;
//   }

//   if (result.face_count == 0) {
//     result.success = false;
//     strcpy(result.error_message, "No faces found in OBJ file");
//     printf("ERROR: %s\n", result.error_message);
//     return result;
//   }

//   result.success = true;
//   printf("=== Successfully loaded OBJ file! ===\n");
//   printf("  Vertices: %d\n", result.vertex_count);
//   printf("  Faces: %d\n", result.face_count);
//   if (has_transforms) {
//     printf("  Applied transforms: scale(%.1f,%.1f,%.1f) pos(%.1f,%.1f,%.1f) "
//            "rot(%.0f°,%.0f°,%.0f°)\n",
//            scale.x, scale.y, scale.z, translation.x, translation.y,
//            translation.z, rotation.x * 180.0 / M_PI, rotation.y * 180.0 /
//            M_PI, rotation.z * 180.0 / M_PI);
//   }
//   printf("=====================================\n");

//   return result;
// }

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